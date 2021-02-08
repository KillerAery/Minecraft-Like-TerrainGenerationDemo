// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/TerrianGenerateSetting.h"


/**
 * 
 */
class TERRIANGENERATEDEMO_API BiomeGenerator
{
public:
	BiomeGenerator();
	~BiomeGenerator();
	static BiomeType GetBiomeType(FVector2D BlockPosition);

	static void GenerateBiome(FVector2D ChunkPosition, BiomeType  BlocksBiome[MaxBlocksWidth][MaxBlocksWidth]);
};
  