#pragma once

UENUM()
enum class EStarConfirmType : uint8
{
	Yes,
	No
};

UENUM()
enum class EStarValidType : uint8
{
	Valid,
	Invalid
};

UENUM()
enum class EStarSuccessType : uint8
{
	Successful,
	Failed
};

UENUM()
enum class EStarCountDownActionInput : uint8
{
	Start,
	Cancel
};

UENUM()
enum class EStarCountDownActionOutput : uint8
{
	Updated,
	Completed,
	Cancelled
};

UENUM(BlueprintType)
enum class EStarGameDifficulty : uint8
{
	Easy,
	Normal,
	Hard,
	VeryHard
};

UENUM(BlueprintType)
enum class EStarInputMode : uint8
{
	GameOnly,
	UIOnly
};