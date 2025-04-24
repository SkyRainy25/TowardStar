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
	// �̹� �浹�� ���� �迭�� �ִٸ� return
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}
	// ������ �߰�.
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
	// OwningPawn�� ������ �� ��ġ�� CollisionBox�� ������
	check(OwningEnemyCharacter);

	UBoxComponent* LeftHandCollisionBox = OwningEnemyCharacter->GetLeftHandCollisionBox();
	UBoxComponent* RightHandCollisionBox = OwningEnemyCharacter->GetRightHandCollisionBox();

	check(LeftHandCollisionBox && RightHandCollisionBox);

	// ToggleDamage Ÿ�Կ� ����, �ش��ϴ� ��ġ�� CollsionBox�� Collision�� ����.
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
		OverlappedActors.Empty();	// ���� �ʱ�ȭ
	}
}
