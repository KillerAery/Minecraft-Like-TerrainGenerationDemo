﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "NoiseTool.h"

uint32 NoiseTool::hash11(int32 position)
{
	const unsigned int BIT_NOISE1 = 0x85297A4D;
	const unsigned int BIT_NOISE2 = 0x68E31DA4;
	const unsigned int BIT_NOISE3 = 0x1B56C4E9;
	unsigned int mangled = position;
	mangled *= BIT_NOISE1;
	mangled ^= (mangled >> 8);
	mangled += BIT_NOISE2;
	mangled ^= (mangled << 8);
	mangled *= BIT_NOISE3;
	mangled ^= (mangled >> 8);
	return mangled;
}

FVector2D NoiseTool::hash22(FVector2D position2D)
{
	FVector2D v(hash11(0x651A6BE3 * (int32)position2D.X)%1024-512, hash11((int32)position2D.Y)%1024-512);
	v/= (int32)v.Size();
	return v;
}

uint32 NoiseTool::hash21(FVector2D position2D)
{
	return hash11(0x651A6BE1 * (int32)position2D.X + (int32)position2D.Y);
}

FVector NoiseTool::hash33(FVector position3D)
{
	return FVector(hash11(position3D.X), hash11(position3D.Y),hash11(position3D.Z));
}

uint32 NoiseTool::hash31(FVector position3D)
{
	return hash11(position3D.X * 0x651A6BE6 - position3D.Y * 0xCB251062 + position3D.Z);
}

float NoiseTool::grad(FVector2D vertex, FVector2D position2D)
{
	return FVector2D::DotProduct(hash22(vertex), position2D);
}

float NoiseTool::perlinNoise(FVector2D position2D,int32 crystalSize)
{
	position2D /= crystalSize;
	FVector2D pi = FVector2D(floor(position2D.X), floor(position2D.Y));

	FVector2D pf = position2D - pi;
	FVector2D w = pf * pf * (FVector2D(3.0f, 3.0f) - 2.0f * pf);

	FVector2D vertex[4] = { {pi.X,pi.Y},{pi.X + 1.0f,pi.Y},{pi.X,pi.Y + 1.0f},{pi.X + 1.0f,pi.Y + 1.0f} };

	return FMath::Lerp(
		FMath::Lerp(grad(vertex[0], pf),
			grad(vertex[1], pf - FVector2D(1.0f, 0.0f)),
			w.X),
		FMath::Lerp(grad(vertex[2], pf - FVector2D(0.0f, 1.0f)),
			grad(vertex[3], pf - FVector2D(1.0f, 1.0f)),
			w.X),
		w.Y);
}

//float NoiseTool::valueNoise(FVector2D position2D)
//{
//	FVector2D pi = FVector2D(floor(position2D.X), floor(position2D.Y));
//
//	FVector2D pf = position2D - pi;
//	FVector2D w = pf * pf * (FVector2D(3.0f, 3.0f) - 2.0f * pf);
//
//	FVector2D vertex[4] = { {pi.X,pi.Y},{pi.X + 1,pi.Y},{pi.X,pi.Y + 1},{pi.X + 1,pi.Y + 1} };
//
//	return FMath::Clamp<float>(FMath::Lerp(
//		FMath::Lerp(hash21(vertex[0]) % 17,
//			hash21(vertex[1]) % 17,
//			w.X),
//		FMath::Lerp(hash21(vertex[2]) % 17,
//			hash21(vertex[3]) % 17,
//			w.X),
//		w.Y), 0, 255);
//}