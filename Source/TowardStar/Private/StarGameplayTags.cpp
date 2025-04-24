// SkyRain


#include "StarGameplayTags.h"

namespace StarGameplayTags
{
	/** Input Tags **/
	// InputTag_Move : C++ 에서 사용할 변수
	// "InputTag.Move" => GameplayTag(태그)
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipBlade, "InputTag.EquipBlade");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UnequipBlade, "InputTag.UnequipBlade");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_LightAttack_Blade, "InputTag.LightAttack.Blade");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_HeavyAttack_Blade, "InputTag.HeavyAttack.Blade");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Roll, "InputTag.Roll");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_SwitchTarget, "InputTag.SwitchTarget");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SpecialWeaponAbility_Light, "InputTag.SpecialWeaponAbility.Light");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SpecialWeaponAbility_Heavy, "InputTag.SpecialWeaponAbility.Heavy");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_PickUp_Stones, "InputTag.PickUp.Stones");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SpecialAbility_FireTornado, "InputTag.SpecialAbility.FireTornado");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_MustBeHeld, "InputTag.MustBeHeld");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_MustBeHeld_Block, "InputTag.MustBeHeld.Block");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable, "InputTag.Toggleable");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable_TargetLock, "InputTag.Toggleable.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable_Rage, "InputTag.Toggleable.Rage");

	/** Player tags **/
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip_Blade, "Player.Ability.Equip.Blade");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Unequip_Blade, "Player.Ability.Unequip.Blade");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Light_Blade, "Player.Ability.Attack.Light.Blade");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Heavy_Blade, "Player.Ability.Attack.Heavy.Blade");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_HitPause, "Player.Ability.HitPause");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Roll, "Player.Ability.Roll");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Block, "Player.Ability.Block");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_TargetLock, "Player.Ability.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Rage, "Player.Ability.Rage");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialWeaponAbility_Light, "Player.Ability.SpecialWeaponAbility.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialWeaponAbility_Heavy, "Player.Ability.SpecialWeaponAbility.Heavy");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_PickUp_Stones, "Player.Ability.PickUp.Stones");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialAbility_FireTornado, "Player.Ability.SpecialAbility.FireTornado");

	UE_DEFINE_GAMEPLAY_TAG(Player_Cooldown_SpecialWeaponAbility_Light, "Player.Cooldown.SpecialWeaponAbility.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_Cooldown_SpecialWeaponAbility_Heavy, "Player.Cooldown.SpecialWeaponAbility.Heavy");
	UE_DEFINE_GAMEPLAY_TAG(Player_Cooldown_SpecialWeaponAbility_FireTornado, "Player.Cooldown.SpecialWeaponAbility.FireTornado");

	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Blade, "Player.Weapon.Blade");

	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip_Blade, "Player.Event.Equip.Blade");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip_Blade, "Player.Event.Unequip.Blade");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_HitPause, "Player.Event.HitPause");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SuccessfulBlock, "Player.Event.SuccessfulBlock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwitchTarget_Left, "Player.Event.SwitchTarget.Left");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwitchTarget_Right, "Player.Event.SwitchTarget.Right");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_ActivateRage, "Player.Event.ActivateRage");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_AOE, "Player.Event.AOE");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_ConsumeStones, "Player.Event.ConsumeStones");

	UE_DEFINE_GAMEPLAY_TAG(Player_Status_JumpToFinisher, "Player.Status.JumpToFinisher");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rolling, "Player.Status.Rolling");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Blocking, "Player.Status.Blocking");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_TargetLock, "Player.Status.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_Activating, "Player.Status.Rage.Activating");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_Active, "Player.Status.Rage.Active");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_Full, "Player.Status.Rage.Full");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_None, "Player.Status.Rage.None");

	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Light, "Player.SetByCaller.AttackType.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Heavy, "Player.SetByCaller.AttackType.Heavy");

	/** Enemy tags **/
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Melee, "Enemy.Ability.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Ranged, "Enemy.Ability.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_SummonEnemies, "Enemy.Ability.SummonEnemies");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_SpawnItem, "Enemy.Ability.SpawnItem");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_SpawnStone, "Enemy.Ability.SpawnStone");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_SpawnObstacle, "Enemy.Ability.SpawnObstacle");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_ChargeJump, "Enemy.Ability.ChargeJump");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Heal, "Enemy.Ability.Heal");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_EnergyBall, "Enemy.Ability.EnergyBall");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Cooldown_SpawnObstacle, "Enemy.Cooldown.SpawnObstacle");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Weapon, "Enemy.Weapon");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_SummonEnemies, "Enemy.Event.SummonEnemies");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Strafing, "Enemy.Status.Strafing");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_UnderAttack, "Enemy.Status.UnderAttack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Unblockable, "Enemy.Status.Unblockable");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Health_Full, "Enemy.Status.Health.Full");

	/** Shared tags **/
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_HitReact, "Shared.Ability.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death");	

	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_MeleeHit, "Shared.Event.MeleeHit");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_HitReact, "Shared.Event.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_SpawnProjectile, "Shared.Event.SpawnProjectile");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_SpawnObstacle, "Shared.Event.SpawnObstacle");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_ChargeJump, "Shared.Event.ChargeJump");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_Heal, "Shared.Event.Heal");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_SpawnEnergyBall, "Shared.Event.SpawnEnergyBall");

	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage");

	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Dead, "Shared.Status.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Front, "Shared.Status.HitReact.Front");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Left, "Shared.Status.HitReact.Left");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Right, "Shared.Status.HitReact.Right");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Back, "Shared.Status.HitReact.Back");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Invincible, "Shared.Status.Invincible");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Healing, "Shared.Status.Healing");

	/** Game Data tags **/
	UE_DEFINE_GAMEPLAY_TAG(GameData_Level_SurvivalGameModeMap, "GameData.Level.SurvivalGameModeMap");
	UE_DEFINE_GAMEPLAY_TAG(GameData_Level_AdventureGameModeMap, "GameData.Level.AdventureGameModeMap");
	UE_DEFINE_GAMEPLAY_TAG(GameData_Level_MainMenuMap, "GameData.Level.MainMenuMap");
	
	UE_DEFINE_GAMEPLAY_TAG(GameData_SaveGame_Slot_1, "GameData.SaveGame.Slot.1");
	UE_DEFINE_GAMEPLAY_TAG(GameData_SaveGame_Slot_2, "GameData.SaveGame.Slot.2");
	UE_DEFINE_GAMEPLAY_TAG(GameData_SaveGame_Slot_3, "GameData.SaveGame.Slot.3");
}

