// SkyRain


#include "Components/Combat/EnemyCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "StarGameplayTags.h"
#include "StarFunctionLibrary.h"
#include "Characters/StarEnemyCharacter.h"
#include "Components/BoxComponent.h"

#include "StarDebugHelper.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	// 이미 충돌한 액터 배열에 있다면 return
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}
	// 없으면 추가.
	OverlappedActors.AddUnique(HitActor);

	//TODO:: Implement block check
	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UStarFunctionLibrary::NativeDoesActorHaveTag(HitActor, StarGameplayTags::Player_Status_Blocking);
	const bool bIsMyAttackUnblockable = UStarFunctionLibrary::NativeDoesActorHaveTag(GetOwningPawn(), StarGameplayTags::Enemy_Status_Unblockable);

	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		//TODO::check if the block is valid
		bIsValidBlock = UStarFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;	

	if (bIsValidBlock)
	{
		//TODO::Handle successful block
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			StarGameplayTags::Player_Event_SuccessfulBlock,
			EventData
		);
	}
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			StarGameplayTags::Shared_Event_MeleeHit,
			EventData
		);
	}
}

void UEnemyCombatComponent::ToggleBodyCollsionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	AStarEnemyCharacter* OwningEnemyCharacter = GetOwningPawn<AStarEnemyCharacter>();
	// OwningPawn이 가지는 각 위치의 CollisionBox를 가져옴
	check(OwningEnemyCharacter);

	UBoxComponent* LeftHandCollisionBox = OwningEnemyCharacter->GetLeftHandCollisionBox();
	UBoxComponent* RightHandCollisionBox = OwningEnemyCharacter->GetRightHandCollisionBox();

	check(LeftHandCollisionBox && RightHandCollisionBox);

	// ToggleDamage 타입에 따라, 해당하는 위치의 CollsionBox의 Collision을 설정.
	switch (ToggleDamageType)
	{
	case EToggleDamageType::LeftHand:
		LeftHandCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		break;

	case EToggleDamageType::RightHand:
		RightHandCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		break;

	default:
		break;
	}

	if (!bShouldEnable)
	{
		OverlappedActors.Empty();	// 액터 초기화
	}
}
