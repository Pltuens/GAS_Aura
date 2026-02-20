 // Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyCharacter.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS_Aura/GAS_Aura.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/Widget/AuraUserWidget.h"

 AEnemyCharacter::AEnemyCharacter()
 {
 	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
 	
 	AbilitySystemComponent=CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
 	AbilitySystemComponent->SetIsReplicated(true);
 	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
 	
 	AttributeSet=CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
 	
 	HealthBar=CreateDefaultSubobject<UWidgetComponent>("HealthBar");
 	HealthBar->SetupAttachment(GetRootComponent());                              
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

 int32 AEnemyCharacter::GetPlayerLevel()
 {
 	return Level;
 }

 void AEnemyCharacter::Die()
 {
 	 SetLifeSpan(LifeSpan);
	 Super::Die();
 }

 void AEnemyCharacter::BeginPlay()
 {
	Super::BeginPlay();
 	GetCharacterMovement()->MaxWalkSpeed=BaseWalkSpeed;
 	InitAbilityActorInfo();
 	UAuraAbilitySystemLibrary::GiveStartupAbilities(this,AbilitySystemComponent);
 	
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
 	GetCharacterMovement()->MaxWalkSpeed=NewCount ? 0.f : BaseWalkSpeed;
 }

 void AEnemyCharacter::InitAbilityActorInfo()
 {
 	AbilitySystemComponent->InitAbilityActorInfo(this,this);
 	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
 	
 	InitializeDefaultAttributes();
 }

 void AEnemyCharacter::InitializeDefaultAttributes() const
 {
	 UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this,CharacterClass,Level,AbilitySystemComponent);
 }
