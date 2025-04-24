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
	// 충돌시 델리게이트 바인딩.
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
	
	// ProjectileDamagePolicy가 OnBeginOverlap인 경우
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		// 캐릭터의 Projectile에 대한 Collision 설정을 = overlap으로 바꾸고
		// 델리게이트에 의해 OnProjectileBeginOverlap 호출
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

// 벽이나 구조물등에 충돌할 때 호출하는 함수.
void AStarProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// FX 출력
	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

	APawn* HitPawn = Cast<APawn>(OtherActor);
	// HitPawn이 없거나, 같은 팀인 경우 => Destroy + 종료
	if (!HitPawn || !UStarFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		Destroy();
		return;
	}

	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UStarFunctionLibrary::NativeDoesActorHaveTag(HitPawn, StarGameplayTags::Player_Status_Blocking);
	// Player의 Block이 유효한가?
	if (bIsPlayerBlocking)
	{
		bIsValidBlock = UStarFunctionLibrary::IsValidBlock(this, HitPawn);
	}
	// 유효하다면, 성공적으로 막았음을 SendGameplayEventToActor를 통해 UE에 전달.
	
	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;

	if (bIsValidBlock)
	{
		// UAbilitySystemBlueprintLibrary::SendGameplayEventToActor를 통해서 Tag를 넘겨주고
		// Wait Gameplay Event 를 통해 실행.(Broadcast -> Listener 방식)
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			this,
			StarGameplayTags::Player_Event_SuccessfulBlock,
			Data
		);
	}
	else
	{
		// 그렇지 않다면 데미지 처리
		HandleApplyProjectileDamage(HitPawn, Data);
	}

	Destroy();
}

void AStarProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 기존에 Overlapped Actor라면 return
	if (OverlappedActors.Contains(OtherActor))
		return;

	OverlappedActors.AddUnique(OtherActor);	// 추가

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;

		// 적대적일 경우
		if (UStarFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			HandleApplyProjectileDamage(HitPawn, Data);	// 데미지 적용
		}
	}
}

void AStarProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	//ProjectileDamageEffectSpecHandle 유효성 검사
	checkf(ProjectileDamageEffectSpecHandle.IsValid(), TEXT("Forgot to assign a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());

	// 데미지 처리 부분
	const bool bWasApplied = UStarFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);

	// 성공적으로 데미지가 적용되었을 경우
	if (bWasApplied)
	{
		// 블루프린트로 Tag와 함께 알림.
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			StarGameplayTags::Shared_Event_HitReact,
			InPayload
		);
	}
}
