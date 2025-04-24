// SkyRain


#include "Items/Weapons/StarWeaponBase.h"
#include "Components/BoxComponent.h"
#include "StarFunctionLibrary.h"

#include "StarDebugHelper.h"

// Sets default values
AStarWeaponBase::AStarWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//StaticWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticWeaponMesh"));

	SkeletalWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalWeaponMesh"));
	SetRootComponent(SkeletalWeaponMesh);
	
	StaticWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticWeaponMesh"));
	StaticWeaponMesh->SetupAttachment(GetRootComponent());

	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// �浹�� �߻��� ��, �ݹ��Լ� ���ε�
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
}

void AStarWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instiagtor as the owning pawn for the weapon: %s"), *GetName());

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		// ���� ���� �ƴ� ��쿡�� ����.
		if (UStarFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
		{
			// ExecuteIfBound => Broadcast�� ��� ����� ����
			// �˸�(��۱��/ = ExecuteIfBound, Broadcast)
			// ����(�˸��� ���� �� ���ε��� �ݹ��Լ� ȣ��)
			// PawnCombatComponent�� Weapon�� ����, ���ε� �� �ݹ��Լ��� ����.
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}

		//TODO:Implement hit check for enemy characters
	}

}

void AStarWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instiagtor as the owning pawn for the weapon: %s"), *GetName());

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UStarFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
		{
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
		}

		//TODO:Implement hit check for enemy characters
	}
}

