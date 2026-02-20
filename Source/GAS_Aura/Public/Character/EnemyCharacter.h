// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Character/BaseCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "EnemyCharacter.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class GAS_AURA_API AEnemyCharacter : public ABaseCharacter,public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AEnemyCharacter();
	/*EnemyInterface*/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/*end EnemyInterface*/
	
	/*CombatInterface*/
	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;
	/*end CombatInterface*/
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	void HitReactTagChanged(const FGameplayTag CallBackTag,int32 NewCount);
	
	UPROPERTY(BlueprintReadOnly,Category="Combat")
	bool bHitReacting=false;
	
	UPROPERTY(BlueprintReadOnly,Category="Combat")
	float BaseWalkSpeed=250.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	float LifeSpan=5.f;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,Category="Character Class Defaults")
	int32 Level=1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,Category="Character Class Defaults")
	ECharacterClass CharacterClass=ECharacterClass::Warrior;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent>HealthBar;
	
};
