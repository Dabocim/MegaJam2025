// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MegaJamCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;


UENUM(BlueprintType)
enum class EMovementType : uint8
{
	EMT_Walking UMETA(DisplayName = "Walking"),
	EMT_Crouching UMETA(DisplayName = "Crouching"),
	EMT_Sprinting UMETA(DisplayName = "Sprinting")
};

UENUM(BlueprintType)
enum class EPrimaryType : uint8
{
	EPT_None UMETA(DisplayName = "None"),
	EPT_NonCombatItem UMETA(DisplayName = "Non-Combat Item"),
	EPT_Melee UMETA(DisplayName = "Melee"),
	EPT_Ranged UMETA(DisplayName = "Ranged")
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AMegaJamCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* UsePrimaryAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* UseQuickItemAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InventoryToggleAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

public:

	/** Constructor */
	AMegaJamCharacter();
	virtual void BeginPlay() override;

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Added inputs */
	void Sprint();
	void EndSprint();

	void StartCrouch();
	void EndCrouch();

	void UsePrimary();

	void Reload();

	void UseQuickItem();

	void InventoryToggle();

	void PlayerInteract();

	void StartAim();
	void EndAim();




	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EMovementType CurrentMovementType = EMovementType::EMT_Walking;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	EPrimaryType CurrentPrimaryType = EPrimaryType::EPT_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float CrouchSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed = 600.f;

	class AActor* PrimaryHand = nullptr;

	class AActor* QuickSlotItem = nullptr;

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION()
	void OnInteractSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	AActor* GetNearestInteractableActor();

	UPROPERTY(EditAnywhere)
	class USphereComponent* InteractSphere;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> InteractableActors;

	UPROPERTY(BlueprintReadOnly)
	AActor* CurrentInteractableFocus;

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

