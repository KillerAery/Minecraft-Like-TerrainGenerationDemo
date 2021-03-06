﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Model/TerrianGenerationMode.h"
#include "Core/HeightGenerator.h"
#include "Core/CaveGenerator.h"
#include "Core/TemperatureGenerator.h"
#include "Core/HumidityGenerator.h"
#include "Core/BiomeGenerator.h"
#include "Core/TreeGenerator.h"
#include "Tool/NoiseTool.h"
#include "TerrianGenerateDemoHUD.h"
#include "TerrianGenerateDemoCharacter.h"


ATerrianGenerationMode::ATerrianGenerationMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATerrianGenerateDemoHUD::StaticClass();
}

void ATerrianGenerationMode::BeginPlay()
{
	Super::BeginPlay();
}

void ATerrianGenerationMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATerrianGenerationMode::SetCameraLoaction(FVector location)
{
	ChunksCenterPosition = FVector2D(
		(int32)(location.X / (MaxBlocksWidth * 100) - 0.5f),
		(int32)(location.Y / (MaxBlocksWidth * 100) - 0.5f));
}

void ATerrianGenerationMode::UpdateChunks()
{
	//生成新Chunk
	for (int i = 0; i < ChunkSize; ++i)
	for (int j = 0; j < ChunkSize; ++j) 
	{
		FVector2D f = FVector2D(ChunksCenterPosition.X + (i - Center),
					  			ChunksCenterPosition.Y + (j - Center));

		if (NeedChunk(f))
			GenerateChunk(f);
	}
}

bool ATerrianGenerationMode::NeedChunk(FVector2D chunkPosition)
{
	//是否已存在Chunks列表内
	if(Chunks.FindByPredicate(
		[chunkPosition](Chunk& chunk){
			return FVector2D::DistSquared(chunkPosition,chunk.ChunkPosition)<0.001f;
		}))
	{
		return false;
	}

	//不存在Chunk列表，则判断是否在加载范围
	FVector2D d = chunkPosition - ChunksCenterPosition;
	return abs((int32)(d.X)) < LoadRadius && abs((int32)fabs(d.Y)) < LoadRadius;
}

void ATerrianGenerationMode::GenerateChunk(FVector2D chunkPosition)
{
	int32 index = Chunks.Add(Chunk(chunkPosition));
	Chunk& chunk = Chunks[index];
	//生成高度
	HeightGenerator::GenerateHeight(chunk);
	//生成洞穴
	CaveGenerator::GenerateCave(chunk,this->Info);
	//生成温度
	TemperatureGenerator::GenerateTemperature(chunk);
	//生成湿度
	HumidityGenerator::GenerateHumidity(chunk);
	//生成生物群落属性
	BiomeGenerator::GenerateBiome(chunk);
	//生成植被
	TreeGenerator::GenerateTree(chunk,this->Info);

	//生成特殊方块
	for(auto& itr : Info.SpecialBlocksID){
		FVector v = NoiseTool::UnIndex(itr.Key);
		FVector BlockPosition = FVector(
			v.X,
			v.Y,
			v.Z);
		CreateBlock(itr.Value,BlockPosition);
	}
	Info.SpecialBlocksID.Reset();

	const int32 TRICK_EDGE_HEIGH = 3;
	//生成地形方块
	for (int i = 0; i < 16; ++i)
	for (int j = 0; j < 16; ++j) 
	{	
		for (int k = chunk.BlocksHeight[i][j]; k >= 0; --k)
		{
			if(i == 0 || j == 0 || i == 15 || j == 15){
				if(chunk.BlocksHeight[i][j]-k > TRICK_EDGE_HEIGH)break;
			}
			else if(k < chunk.BlocksHeight[i][j]){
				const int32 dx[4] = {-1,1,0,0};
				const int32 dy[4] = {0,0,-1,1};

				bool flag = false;
				for(int32 d = 0;d<4;++d){
					if(k>chunk.BlocksHeight[i+dx[d]][j+dy[d]]){
						flag = true;
						break;
					}
					auto f = Blocks.Find(NoiseTool::Index(chunk.ChunkPosition.X*16+i+dx[d],chunk.ChunkPosition.Y*16+j+dy[d],k));
					if(f){
						flag = true;
						break;
					}
				}
				if(!flag)break;
			}

			int32 targetBlockID;
			/* None = 0 雪地 Snow = 10 草地 Green = 1
			泥地 Dry = 3 石地 Stone = 4 沙漠 Desert = 5
			*/
			switch (chunk.BlocksBiome[i][j]){
				case 1:targetBlockID = 10;break;
				case 2:targetBlockID = 1;break;
				case 3:targetBlockID = 3;break;
				case 4:targetBlockID = 5;break;
				case 5:targetBlockID = 4;break;
				default:break;
			}

			//随机泥土
			if (k < chunk.BlocksHeight[i][j] || (rand() % 255 >= 250)){
				targetBlockID = 3;
			}

			//边缘高度选择 地表、地下
			if(chunk.BlocksHeight[i][j]-k>=2){
				targetBlockID = 2;
			}

			FVector BlockPosition = FVector(
				chunkPosition.X*16 + i,
				chunkPosition.Y*16 + j,
			k);
				
			CreateBlock(targetBlockID,BlockPosition);
		}
	}
}


int32 ATerrianGenerationMode::GetHeight(FVector2D position){
	return 0;
}

bool ATerrianGenerationMode::CreateBlock(int32 id, FVector blockIndexPosition)
{
	if (id < 0 || id > MAX_BLOCKS_NUM) {return false;}
	uint64 index = NoiseTool::Index(blockIndexPosition.X,blockIndexPosition.Y,blockIndexPosition.Z);
	auto result = Blocks.Find(index);
	//已存在方块，失败
	if(result)return false;
	
	//雪是特殊方块，处理特殊高度
	if(id==24)blockIndexPosition.Z-=0.5f;

	//挖空方块，处理特殊空气方块
	if(id==0){
		Blocks.Add(index,nullptr);
		return true;
	}

	//创建方块
	ABlock* block = GetWorld()->SpawnActor<ABlock>(blockIndexPosition*100, FRotator::ZeroRotator);
	block->InitByBlockID(id);
	Blocks.Add(index,block);

	return true;
}



