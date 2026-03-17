#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "BaseCharacter.generated.h"

class UAnimMontage;

UCLASS(Abstract)
class GAS_AURA_API ABaseCharacter : public ACharacter,public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}
	
	/*CombatInterface*/	
	virtual void Die() override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	/*end CombatInterface*/	
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
protected:
	
	bool bDead = false;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	
	UPROPERTY(EditAnywhere,Category="Combat")
	FName WeaponTipSocketName;
	
	UPROPERTY(EditAnywhere,Category="Combat")
	FName LeftHandSocketName;
	
	UPROPERTY(EditAnywhere,Category="Combat")
	FName RightHandSocketName;
	
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	virtual void InitAbilityActorInfo();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Attribute")
	TSubclassOf<UGameplayEffect>DefaultPrimaryAttributes;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Attribute")
	TSubclassOf<UGameplayEffect>DefaultSecondaryAttributes;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Attribute")
	TSubclassOf<UGameplayEffect>DefaultVitalAttributes;
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect>GameplayEffectClass,float Level) const;
	virtual void InitializeDefaultAttributes() const;
	
	void AddCharacterAbilities();
	
	/*溶解效果*/
	
	void Dissolve();
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* MaterialInstanceDynamic);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* MaterialInstanceDynamic);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;
	
private:
	
	UPROPERTY(EditAnywhere,Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>>StartupAbilities;
	
	UPROPERTY(EditAnywhere,Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

};
