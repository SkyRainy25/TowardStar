// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "StarAttributeSet.generated.h"

// Attribute를 Get/ Set 할 수 있게 하는 Macro 
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthStatusChangedDelegate, bool, bHealthIsFull);

class IPawnUIInterface;
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UStarAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UStarAttributeSet();

	// 어떤 Attribute가 Modify(변경) 되었는지 찾는 함수?
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintAssignable)
	FOnHealthStatusChangedDelegate OnHealthIsFull;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UStarAttributeSet, CurrentHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UStarAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Rage")
	FGameplayAttributeData CurrentRage;
	ATTRIBUTE_ACCESSORS(UStarAttributeSet, CurrentRage);

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(UStarAttributeSet, MaxRage);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UStarAttributeSet, Attack);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UStarAttributeSet, Defense);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UStarAttributeSet, DamageTaken);

private:
	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;
};
