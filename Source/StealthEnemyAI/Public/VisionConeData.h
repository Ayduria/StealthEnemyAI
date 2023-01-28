#pragma once

#include "VisionConeData.generated.h"

USTRUCT()
struct FConeQueryData{

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float CircleRadius = 500.f;

	UPROPERTY(EditAnywhere)
	float SpaceBetween = 50.f;

	UPROPERTY(EditAnywhere)
	int NumberOfPoints = 50;

	UPROPERTY(EditAnywhere)
	float ArcAngle = 90.f;
};

/**
 *
 */
UCLASS()
class STEALTHENEMYAI_API UVisionConeData : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FConeQueryData ConeQueryData;

};