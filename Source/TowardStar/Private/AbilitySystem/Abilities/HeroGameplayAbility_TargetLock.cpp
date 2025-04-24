// SkyRain


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/StarHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/StarWidgetBase.h"
#include "Controllers/StarPlayerController.h"
#include "StarFunctionLibrary.h"
#include "StarGameplayTags.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"

#include "StarDebugHelper.h"


void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	InitTargetLockMovement();
	InitTargetLockMappingContext();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetTargetLockMovement();
	ResetTargetLockMappingContext();
	CleanUp();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	// LockedActor�� ���ų�, LockedActor�� �׾��ų�, �÷��̾ �׾��ٸ�
	if(!CurrentLockedActor || UStarFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor, StarGameplayTags::Shared_Status_Dead) || 
		UStarFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), StarGameplayTags::Shared_Status_Dead))
	{
		CancelTargetLockAbility();	// ���
		return;
	}

	SetTargetLockWidgetPosition();

	// �÷��̾��� ���� Ȯ��(Rolling X + Blocking X)
	const bool bShouldOverrideRotation = 
		!UStarFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), StarGameplayTags::Player_Status_Rolling)
		&&
		!UStarFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), StarGameplayTags::Player_Status_Blocking);

	if (bShouldOverrideRotation)
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			GetHeroCharacterFromActorInfo()->GetActorLocation(),
			CurrentLockedActor->GetActorLocation()
		);

		LookAtRot -= FRotator(TargetLockCameraOffsetDistance, 0.f, 0.f);

		const FRotator CurrentControlRot = GetStarPlayerControllerFromActorInfo()->GetControlRotation();
		const FRotator TargetRot = FMath::RInterpTo(CurrentControlRot, LookAtRot, DeltaTime, TargetLockRotationInterpSpeed);

		// PC �� LockedActor�� ��Ŀ��(ī�޶� ȸ��)
		GetStarPlayerControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));
		// LockedActor�� ���� ĳ���͸� ȸ��.
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f, TargetRot.Yaw, 0.f));
	}
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	GetAvailableActorsToLock();

	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock = nullptr;

	GetAvailableActorsAroundTarget(ActorsOnLeft, ActorsOnRight);

	// ������ ���
	if (InSwitchDirectionTag == StarGameplayTags::Player_Event_SwitchTarget_Left)
	{
		// ���� �迭���� ���� ����� Actor�� ������ ����
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnLeft);
	}
	else // �������� ��� 
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnRight);
	}

	// Update LockedActor
	if (NewTargetToLock)
	{
		CurrentLockedActor = NewTargetToLock;
	}
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();

	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock);

	if (CurrentLockedActor)	// ���� Lock �� �� �ִ� ���Ͱ� �Ҵٸ�?
	{
		//Debug::Print(CurrentLockedActor->GetActorNameOrLabel());
		DrawTargetLockWidget();

		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();	// ���
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	AvailableActorsToLock.Empty();
	TArray<FHitResult> BoxTraceHits;

	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),	// Start Point
		// End Point = StartPoint + TraceDistance(Tarce�� �Ÿ� * PC �� �ٶ󺸴� ����)
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetHeroCharacterFromActorInfo()->GetActorForwardVector() * BoxTraceDistance,	// 
		TraceBoxSize / 2.f,	// Box half size
		GetHeroCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(),	// (Trace)Box�� ���ϴ� ����.
		BoxTraceChannel,	// Obejct Type
		false,
		TArray<AActor*>(),	// Actors to Ignore
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,	// where it shows debug sphere?
		BoxTraceHits,	// OutParam
		true			// �ڽ��� ����.
		);

	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		// Trace�� �浹�� ���Ͱ�
		if (AActor* HitActor = TraceHit.GetActor())
		{
			// ���� ��쿡�� 
			if (HitActor != GetHeroCharacterFromActorInfo())
			{
				// Lock ������ Actor �迭�� �߰�.
				AvailableActorsToLock.AddUnique(HitActor);

				//Debug::Print(HitActor->GetActorNameOrLabel());
			}
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = 0.f;
	// ���� ����� �Ÿ��� ���͸� ��ȯ
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(), InAvailableActors, ClosestDistance);
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight)
{
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	const FVector PlayerLocation = GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized = (CurrentLockedActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		// CurrentLockedActor �� AvailableActor(�ٲ� Ÿ���� ���� ���)
		if (!AvailableActor || AvailableActor == CurrentLockedActor) continue;

		const FVector PlayerToAvailableNormalized = (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

		// PC - CurrentLocked Actor & PC - AvailableActor�� ���͸� �����Ͽ� ��ġ �ľ�
		const FVector CrossResult = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);

		if (CrossResult.Z > 0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forgot to assign a valid widget class in Blueprint"));

		DrawnTargetLockWidget = CreateWidget<UStarWidgetBase>(GetStarPlayerControllerFromActorInfo(), TargetLockWidgetClass);

		check(DrawnTargetLockWidget);

		DrawnTargetLockWidget->AddToViewport();
	}
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	// �� ��� X or TargetLock Widget X => Cancel
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}

	FVector2D ScreenPosition;
	// World Location ������ ��ġ�� �����ϱ� ���� �Լ�.
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetStarPlayerControllerFromActorInfo(),
		CurrentLockedActor->GetActorLocation(),
		ScreenPosition,
		true
	);

	// ������ ũ�� ���� X?
	if (TargetLockWidgetSize == FVector2D::ZeroVector)	
	{
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
			[this](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					// UE���� ���� ������ sizebox�� ũ�⸦ override
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			}
		);
	}

	// ������ ��ġ�� ����
	ScreenPosition -= (TargetLockWidgetSize / 2.f);

	// Draw widget on ScreenPosition (�ʱ⿡��)
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition, false);
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	// TargetLock�� �ϱ� ���� MaxWalkSpeed�� ����(TargetLock�� Ǯ����, ���� ���·� ���ư��� ����)
	CachedDefaultMaxWalkSpeed = GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed;

	// TargetLock�� �� ��, �ӵ� ����.
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = TargetLockMaxWalkSpeed;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	const ULocalPlayer* LocalPlayer = GetStarPlayerControllerFromActorInfo()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem)

	// MappingContext �߰�, Default�� priority = 0 �̱� ������, �� ū���� �켱���� ����.
	Subsystem->AddMappingContext(TargetLockMappingContext, 3);
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void UHeroGameplayAbility_TargetLock::CleanUp()
{
	// �迭 �� ���� Lock ���� �ʱ�ȭ
	AvailableActorsToLock.Empty();

	CurrentLockedActor = nullptr;

	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}

	DrawnTargetLockWidget = nullptr;

	TargetLockWidgetSize = FVector2D::ZeroVector;

	CachedDefaultMaxWalkSpeed = 0; 
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	if (CachedDefaultMaxWalkSpeed > 0.f)
	{
		GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
	}
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	if (!GetStarPlayerControllerFromActorInfo())
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = GetStarPlayerControllerFromActorInfo()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem)

	Subsystem->RemoveMappingContext(TargetLockMappingContext);

}
