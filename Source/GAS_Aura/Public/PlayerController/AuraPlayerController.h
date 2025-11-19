// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/EnemyInterface.h"
#include "AuraPlayerController.generated.h"

class IEnemyInterface;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

UCLASS()
class GAS_AURA_API AAuraPlayerController : public APlayerController,public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime)override;
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
private:
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputAction> MoveAction;
	
	void Move(const FInputActionValue&InputActionValue);
	
	void CursorTrace();
	IEnemyInterface*LastActor;
	IEnemyInterface*ThisActor;
};
