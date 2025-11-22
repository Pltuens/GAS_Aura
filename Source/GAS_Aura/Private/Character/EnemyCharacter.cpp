 // Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyCharacter.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "GAS_Aura/GAS_Aura.h"

 AEnemyCharacter::AEnemyCharacter()
 {
 	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
 	
 	AbilitySystemComponent=CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
 	AbilitySystemComponent->SetIsReplicated(true);
 	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
 	
 	AttributeSet=CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
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

void AEnemyCharacter::BeginPlay()
 {
	Super::BeginPlay();
 	AbilitySystemComponent->InitAbilityActorInfo(this,this);
 }