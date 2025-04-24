// SkyRain


#include "Characters/StarHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputSubsystems.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Components/Input/StarInputComponent.h"
#include "StarGameplayTags.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Components/UI/HeroUIComponent.h"
#include "GameModes/StarGameModeBase.h"

#include "StarDebugHelper.h"

AStarHeroCharacter::AStarHeroCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// ���콺 ȸ�� ����
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 200.f;
	SpringArm->SocketOffset = FVector(0.f, 55.f, 65.f);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));

	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
}

UPawnCombatComponent* AStarHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

UPawnUIComponent* AStarHeroCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;
}

UHeroUIComponent* AStarHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

void AStarHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// StartupData�� ������ �ִ� ���
	if (!CharacterStartUpData.IsNull())
	{
		// ����(Synchronous) �ε�
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			int32 AbilityApplyLevel = 1;

			// GameMode�� ���� ���̵� �����͸� ������.
			if (AStarGameModeBase* StarGameMode = GetWorld()->GetAuthGameMode<AStarGameModeBase>())
			{
				switch (StarGameMode->GetCurrentGameDifficulty())
				{
				case EStarGameDifficulty::Easy:
					AbilityApplyLevel = 4;
					Debug::Print(TEXT("Current Difficulty: Easy"));
					break;
				case EStarGameDifficulty::Normal:
					Debug::Print(TEXT("Current Difficulty: Normal"));
					AbilityApplyLevel = 3;
					break;
				case EStarGameDifficulty::Hard:
					Debug::Print(TEXT("Current Difficulty: Hard"));
					AbilityApplyLevel = 2;
					break;
				case EStarGameDifficulty::VeryHard:
					Debug::Print(TEXT("Current Difficulty: VeryHard"));
					AbilityApplyLevel = 1;
					break;
				default:
					break;
				}
			}

			// GrantAbility + ���̵��� �°� AbilityLevel�� �Բ� ����.
			LoadedData->GiveToAbilitySystemComponent(StarAbilitySystemComponent, AbilityApplyLevel);
		}

	}
}

void AStarHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset, TEXT("Forgot to assign a valid data asset as input config"));

	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	UStarInputComponent* StarInputComponent = CastChecked<UStarInputComponent>(PlayerInputComponent);

	// StarInputComponent���� InputTag - CallbackFunc �� ���ε�.
	StarInputComponent->BindNativeInputAction(InputConfigDataAsset, StarGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	StarInputComponent->BindNativeInputAction(InputConfigDataAsset, StarGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

	StarInputComponent->BindNativeInputAction(InputConfigDataAsset, StarGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Triggered, this, &ThisClass::Input_SwitchTargetTriggered);
	StarInputComponent->BindNativeInputAction(InputConfigDataAsset, StarGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Completed, this, &ThisClass::Input_SwitchTargetCompleted);
	StarInputComponent->BindNativeInputAction(InputConfigDataAsset, StarGameplayTags::InputTag_PickUp_Stones, ETriggerEvent::Started, this, &ThisClass::Input_PickUpStonesStarted);

	StarInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
}

void AStarHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AStarHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

	// �յڷ� �̵�
	if (MovementVector.Y != 0.f)
	{
		// ForwardVector�� �����ͼ� AddMovementInput�� ���� �̵�
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		// ForwardVector�� �����ͼ� AddMovementInput�� ���� �̵�
		AddMovementInput(ForwardDirection, MovementVector.Y);
	}

	// �翷 �̵�
	if (MovementVector.X != 0.f)
	{
		// RightVector�� �����ͼ� AddMovementInput�� ���� �̵�
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AStarHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	// LookAxisVector�� ���� ȸ�� 
	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AStarHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	// ����Ī�� ������ ����.
	SwitchDirection = InputActionValue.Get<FVector2D>();
}

void AStarHeroCharacter::Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;

	// SwitchDirection.X �� ���� ���� ����Ī�� ������ �±׷� �����Ͽ� SendEvent
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		SwitchDirection.X > 0.f? StarGameplayTags::Player_Event_SwitchTarget_Right : StarGameplayTags::Player_Event_SwitchTarget_Left,
		Data
	);
}

void AStarHeroCharacter::Input_PickUpStonesStarted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		StarGameplayTags::Player_Event_ConsumeStones,
		Data
	);
}

void AStarHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	StarAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void AStarHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	StarAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}
