// SkyRain


#include "Items/StarProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "StarFunctionLibrary.h"
#include "StarGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "StarDebugHelper.h"

// Sets default values
AStarProjectileBase::AStarProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	// Set Collision
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	// �浹�� ��������Ʈ ���ε�.
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 4.f;


}

// Called when the game starts or when spawned
void AStarProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	// ProjectileDamagePolicy�� OnBeginOverlap�� ���
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		// ĳ������ Projectile�� ���� Collision ������ = overlap���� �ٲٰ�
		// ��������Ʈ�� ���� OnProjectileBeginOverlap ȣ��
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

// ���̳� ������� �浹�� �� ȣ���ϴ� �Լ�.
void AStarProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// FX ���
	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

	APawn* HitPawn = Cast<APawn>(OtherActor);
	// HitPawn�� ���ų�, ���� ���� ��� => Destroy + ����
	if (!HitPawn || !UStarFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		Destroy();
		return;
	}

	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UStarFunctionLibrary::NativeDoesActorHaveTag(HitPawn, StarGameplayTags::Player_Status_Blocking);
	// Player�� Block�� ��ȿ�Ѱ�?
	if (bIsPlayerBlocking)
	{
		bIsValidBlock = UStarFunctionLibrary::IsValidBlock(this, HitPawn);
	}
	// ��ȿ�ϴٸ�, ���������� �������� SendGameplayEventToActor�� ���� UE�� ����.
	
	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;

	if (bIsValidBlock)
	{
		// UAbilitySystemBlueprintLibrary::SendGameplayEventToActor�� ���ؼ� Tag�� �Ѱ��ְ�
		// Wait Gameplay Event �� ���� ����.(Broadcast -> Listener ���)
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			this,
			StarGameplayTags::Player_Event_SuccessfulBlock,
			Data
		);
	}
	else
	{
		// �׷��� �ʴٸ� ������ ó��
		HandleApplyProjectileDamage(HitPawn, Data);
	}

	Destroy();
}

void AStarProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ������ Overlapped Actor��� return
	if (OverlappedActors.Contains(OtherActor))
		return;

	OverlappedActors.AddUnique(OtherActor);	// �߰�

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;

		// �������� ���
		if (UStarFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			HandleApplyProjectileDamage(HitPawn, Data);	// ������ ����
		}
	}
}

void AStarProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	//ProjectileDamageEffectSpecHandle ��ȿ�� �˻�
	checkf(ProjectileDamageEffectSpecHandle.IsValid(), TEXT("Forgot to assign a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());

	// ������ ó�� �κ�
	const bool bWasApplied = UStarFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);

	// ���������� �������� ����Ǿ��� ���
	if (bWasApplied)
	{
		// �������Ʈ�� Tag�� �Բ� �˸�.
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			StarGameplayTags::Shared_Event_HitReact,
			InPayload
		);
	}
}
