// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Model/Chunk.h"

/**
 * 
 */
class TERRIANGENERATEDEMO_API RainGenerator
{
public:
	static void GenerateRain(Chunk& chunk);
private:
	RainGenerator() = delete;
	~RainGenerator() = delete;
};