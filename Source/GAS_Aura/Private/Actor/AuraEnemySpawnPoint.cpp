// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEnemySpawnPoint.h"

#include "Character/EnemyCharacter.h"

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<AEnemyCharacter>(EnemyClass, GetActorTransform());
	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}
