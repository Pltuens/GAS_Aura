// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "Tags/AuraGameplayTags.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectedApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
        // 创建 Spec
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        
        // 既然不再使用 DynamicAbilityTags，我们直接授予 Ability 即可
        // 只要 AbilityClass 是 UAuraGameplayAbility，它内部就已经自带了 StartupInputTag
        GiveAbility(AbilitySpec);
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        // 核心修改：通过 AbilitySpec.Ability (CDO) 来获取并对比标签
        const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability);
        
        if (AuraAbility && AuraAbility->StartupInputTag.MatchesTagExact(InputTag))
        {
            AbilitySpecInputPressed(AbilitySpec);
            if (!AbilitySpec.IsActive())
            {
                TryActivateAbility(AbilitySpec.Handle);
            }
        }
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability);
        
        if (AuraAbility && AuraAbility->StartupInputTag.MatchesTagExact(InputTag))
        {
            AbilitySpecInputReleased(AbilitySpec);
        }
    }
}

void UAuraAbilitySystemComponent::ClientEffectedApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
    const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);
    
    EffectAssetTags.Broadcast(TagContainer);
}