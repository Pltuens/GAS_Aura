// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

/**
 * 静态工厂函数：在蓝图中通过此函数创建异步节点
 * @param AbilitySystemComponent 传入要监听的 ASC
 * @param InCooldownTag 传入要监听的冷却标签（例如 Ability.Skill.Cooldown）
 */
UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
    // 创建异步任务对象实例
    UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
    WaitCooldownChange->ASC = AbilitySystemComponent;
    WaitCooldownChange->CooldownTag = InCooldownTag;
    
    // 安全检查：如果对象无效或标签无效，立即终止任务并返回空
    if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
    {
       WaitCooldownChange->EndTask();
       return nullptr;
    }

    // --- 核心监听逻辑 1：监听标签的添加或移除 ---
    // RegisterGameplayTagEvent 会在标签数量发生变化（从0到1，或从1到0）时触发
    // 使用 AddUObject 将其绑定到本类的 CooldownTagChanged 函数上
    AbilitySystemComponent->RegisterGameplayTagEvent(
       InCooldownTag,
       EGameplayTagEventType::NewOrRemoved).AddUObject(
          WaitCooldownChange,
          &UWaitCooldownChange::CooldownTagChanged);

    // --- 核心监听逻辑 2：监听新的 GameplayEffect 施加 ---
    // 当有新的 GE 作用于自身时，触发 OnActiveEffectAdded，用于获取具体的冷却持续时间
    AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);

    return WaitCooldownChange;
}

/**
 * 任务清理函数：防止内存泄漏
 */
void UWaitCooldownChange::EndTask()
{
    if (!IsValid(ASC)) return;
    
    // 任务结束时，必须取消注册之前绑定的所有委托
    ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

    // 标记对象为可销毁，等待垃圾回收（GC）
    SetReadyToDestroy();
    MarkAsGarbage();
}

/**
 * 当冷却标签数量变化时的回调
 */
void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
    // 如果标签数量变为 0，说明冷却效果已过期或被移除
    if (NewCount == 0)
    {
       // 广播冷却结束事件，蓝图中的 CooldownEnd 引脚会执行
       CooldownEnd.Broadcast(0.f);
    }
}

/**
 * 当有新的 GE 施加到 ASC 身上时的回调
 */
void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    // 获取新施加的 GE 中包含的所有标签
    FGameplayTagContainer AssetTags;
    SpecApplied.GetAllAssetTags(AssetTags);

    FGameplayTagContainer GrantedTags;
    SpecApplied.GetAllGrantedTags(GrantedTags);

    // 判断这个新 GE 是否包含我们正在监听的冷却标签
    if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
    {
       // 创建一个查询条件：寻找拥有该冷却标签的所有激活状态的 GE
       FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
       
       // 获取所有符合条件的 GE 的剩余时间
       TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
       
       if (TimesRemaining.Num() > 0)
       {
          // 逻辑处理：如果存在多个重叠的冷却 GE，取时间最长的那一个
          float TimeRemaining = TimesRemaining[0];
          for (int32 i = 0; i < TimesRemaining.Num(); i++)
          {
             if (TimesRemaining[i] > TimeRemaining)
             {
                TimeRemaining = TimesRemaining[i];
             }
          }
          
          // 广播冷却开始事件，并将剩余时间传递给蓝图引脚
          CooldownStart.Broadcast(TimeRemaining);
       }
    }
}