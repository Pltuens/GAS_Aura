// AuraAbilitySystemGlobals.cpp

#include "AbilitySystem/AuraAbilitySystemGlobals.h"
#include "AuraAbilityTypes.h" // 确保包含了定义你自定义结构体的头文件

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	// 必须返回你自定义的结构体，而不是原生的
	// 假设你的自定义结构体叫 FAuraGameplayEffectContext
	return new FAuraGameplayEffectContext(); 
}