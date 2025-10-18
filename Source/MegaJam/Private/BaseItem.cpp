// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));

}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseItem::ItemInteract_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Interacted with %s"), *GetName());
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

