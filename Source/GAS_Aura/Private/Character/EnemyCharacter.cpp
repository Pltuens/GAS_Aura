 // Fill out your copyright notice in the Description page of Project Settings.
#include "Character/EnemyCharacter.h"
#include "GAS_Aura/GAS_Aura.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/Widget/AuraUserWidget.h"

 AEnemyCharacter::AEnemyCharacter()
 {
 	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
 	
 	AbilitySystemComponent=CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
 	AbilitySystemComponent->SetIsReplicated(true);
 	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
 	
 	bUseControllerRotationPitch = false;
 	bUseControllerRotationRoll = false;
 	bUseControllerRotationYaw = false;
 	GetCharacterMovement()->bUseControllerDesiredRotation = true; 
 	
 	AttributeSet=CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
 	
 	HealthBar=CreateDefaultSubobject<UWidgetComponent>("HealthBar");
 	HealthBar->SetupAttachment(GetRootComponent());                 
 	
 	BaseWalkSpeed = 250.f;
 }

 void AEnemyCharacter::PossessedBy(AController* NewController)
 {
 	Super::PossessedBy(NewController);
 	
 	if (!HasAuthority()) return;
 	AuraAIController = Cast<AAuraAIController>(NewController);
 	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
 	AuraAIController->RunBehaviorTree(BehaviorTree);
 	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
 	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
 	
 }


 void AEnemyCharacter::HighlightActor()
 {
 	GetMesh()->SetRenderCustomDepth(true);
 	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
 	Weapon->SetRenderCustomDepth(true);
 	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
 }

 void AEnemyCharacter::UnHighlightActor()
 {
 	GetMesh()->SetRenderCustomDepth(false);
 	Weapon->SetRenderCustomDepth(false);
 }

 void AEnemyCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
 {
	 CombatTarget = InCombatTarget;
 }

 AActor* AEnemyCharacter::GetCombatTarget_Implementation() const
 {
	 return CombatTarget;
 }

 int32 AEnemyCharacter::GetPlayerLevel_Implementation()
 {
 	return Level;
 }

 void AEnemyCharacter::Die(const FVector& DeathImpulse)
 {
 	SetLifeSpan(LifeSpan);
 	if (AuraAIController)
 	{
 		if (UBlackboardComponent* BBComp = AuraAIController->GetBlackboardComponent())
 		{
 			BBComp->SetValueAsBool(FName("Dead"), true);
 		}
 	}
 	Super::Die(DeathImpulse);
 }

 void AEnemyCharacter::BeginPlay()
 {
	Super::BeginPlay();
 	GetCharacterMovement()->MaxWalkSpeed=BaseWalkSpeed;
 	InitAbilityActorInfo();
 	if (HasAuthority())
 	{
 			UAuraAbilitySystemLibrary::GiveStartupAbilities(this,AbilitySystemComponent, CharacterClass);
 	}
 	
 	if (UAuraUserWidget*AuraUserWidget=Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
 	{
 		AuraUserWidget->SetWidgetController(this);
 	}
 	const UAuraAttributeSet* AuraAS=Cast<UAuraAttributeSet>(AttributeSet);
 	if (AuraAS)
 	{
 		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
 			[this](const FOnAttributeChangeData& Data)
 			{
 				OnHealthChanged.Broadcast(Data.NewValue);
 			}
 		);
 		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
			 [this](const FOnAttributeChangeData& Data)
			 {
				 OnMaxHealthChanged.Broadcast(Data.NewValue);
			 }
		 );
 	}
 	
 	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact,EGameplayTagEventType::NewOrRemoved).AddUObject(
 		this,
 		&AEnemyCharacter::HitReactTagChanged
 	);
 	
 	OnHealthChanged.Broadcast(AuraAS->GetHealth());
 	OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
 }

void AEnemyCharacter::HitReactTagChanged(const FGameplayTag CallBackTag, int32 NewCount)
 {
 	bHitReacting=NewCount>0;
 	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
 	if (AuraAIController && AuraAIController->GetBlackboardComponent())
 	{
 		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
 	}
 }

 void AEnemyCharacter::InitAbilityActorInfo()
 {
 	AbilitySystemComponent->InitAbilityActorInfo(this,this);
 	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
 	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AEnemyCharacter::StunTagChanged);
	
 	if (HasAuthority())
 	{
 		InitializeDefaultAttributes();
 	}
 	OnASCRegistered.Broadcast(AbilitySystemComponent);
 	
 }

 void AEnemyCharacter::InitializeDefaultAttributes() const
 {
	 UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this,CharacterClass,Level,AbilitySystemComponent);
 }

 void AEnemyCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
 {
	 Super::StunTagChanged(CallbackTag, NewCount);
 	if (AuraAIController && AuraAIController->GetBlackboardComponent())
 	{
 		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStunned"), bIsStunned);
 	}
 }
