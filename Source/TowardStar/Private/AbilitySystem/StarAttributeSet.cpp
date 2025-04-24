// SkyRain


#include "AbilitySystem/StarAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "StarFunctionLibrary.h"
#include "StarGameplayTags.h"
#include "Interfaces/PawnUIInterface.h"
#include "Components/UI/PawnUIComponent.h"
#include "Components/UI/HeroUIComponent.h"

#include "StarDebugHelper.h"

UStarAttributeSet::UStarAttributeSet()
{
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitCurrentRage(1.f);
	InitMaxRage(1.f);
	InitAttack(1.f);
	InitDefense(1.f);
}

void UStarAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		// AvatarActor�� PawnUIInterface�� �����Ҽ� �ִٸ�(����ϰ� �ִٸ�)
		// �� ������ ����.
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}

	checkf(CachedPawnUIInterface.IsValid(), TEXT("%s didn't implement IPawnUIInterface"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());

	UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent();

	checkf(PawnUIComponent, TEXT("Couldn't extract a PawnUIComponent from %s"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());
	
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());

		SetCurrentHealth(NewCurrentHealth);

		if (GetCurrentHealth() == GetMaxHealth())
		{
			OnHealthIsFull.Broadcast(true);
		}
		// �ٲ� Attribute���� ������ ���·� UE�� �˸�(UI�� �Ѱ��ֱ�)
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
	}
	if (Data.EvaluatedData.Attribute == GetCurrentRageAttribute())
	{
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(), 0.f, GetMaxRage());

		SetCurrentRage(NewCurrentRage);

		if (GetCurrentRage() == GetMaxRage())	// Rage�� ����á�� �� => Tag�� ���� ǥ��
		{
			UStarFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), StarGameplayTags::Player_Status_Rage_Full);
		}
		else if (GetCurrentRage() == 0.f)	// Rage�� 0 �϶�
		{
			UStarFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), StarGameplayTags::Player_Status_Rage_None);
		}
		else     // �� �� 
		{
			UStarFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), StarGameplayTags::Player_Status_Rage_Full);
			UStarFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), StarGameplayTags::Player_Status_Rage_None);
		}


		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			// �ٲ� Attribute���� ������ ���·� UE�� �˸�(UI�� �Ѱ��ֱ�)
			HeroUIComponent->OnCurrentRageChanged.Broadcast(GetCurrentRage() / GetMaxRage());
		}
	}
	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float OldHealth = GetCurrentHealth();
		const float Damage = GetDamageTaken();

		const float NewCurrentHealth = FMath::Clamp(OldHealth - Damage, 0.f, GetMaxHealth());

		SetCurrentHealth(NewCurrentHealth);

		//const FString DebugString = FString::Printf(
		//	TEXT("Old Health: %f, Damage Done: %f, NewCurrentHealth: %f"),
		//	OldHealth,
		//	Damage,
		//	NewCurrentHealth
		//);

		//Debug::Print(DebugString, FColor::Green);

		
		//TODO::Notify the UI 
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
		
		//TODO::Handle character death
		if (GetCurrentHealth() == 0.f)
		{
			// Actor�� �ش� Tag�� "���ٸ�" �߰�. => TriggerTag�� �ǰ� TriggerTag�� �߰� �Ǹ� GA_Enemy_DeathBase Ȱ��ȭ.
			UStarFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), StarGameplayTags::Shared_Status_Dead);
		}
	}
}
