// Copyright Epic Games, Inc. All Rights Reserved.

#include "MegaJamCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/SphereComponent.h"
#include "Interactable.h"
#include "BaseItem.h"
#include "MegaJam.h"


AMegaJamCharacter::AMegaJamCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Setup overlap sphere for interaction
	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractSphere->SetupAttachment(RootComponent);
	InteractSphere->SetSphereRadius(200.f);

	// Bind delegates for overlap events

	InteractSphere->OnComponentBeginOverlap.AddDynamic(this, &AMegaJamCharacter::OnInteractSphereOverlapBegin);
	InteractSphere->OnComponentEndOverlap.AddDynamic(this, &AMegaJamCharacter::OnInteractSphereOverlapEnd);




	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMegaJamCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AMegaJamCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		UE_LOG(LogTemp, Warning, TEXT("Binding input actions for '%s'"), *GetNameSafe(this));

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMegaJamCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMegaJamCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMegaJamCharacter::Look);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMegaJamCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMegaJamCharacter::EndCrouch);

		// Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMegaJamCharacter::StartAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMegaJamCharacter::EndAim);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMegaJamCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMegaJamCharacter::EndSprint);

		// Using Primary
		EnhancedInputComponent->BindAction(UsePrimaryAction, ETriggerEvent::Started, this, &AMegaJamCharacter::UsePrimary);

		// Reloading
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMegaJamCharacter::Reload);

		// Using Quick Item
		EnhancedInputComponent->BindAction(UseQuickItemAction, ETriggerEvent::Started, this, &AMegaJamCharacter::UseQuickItem);

		// Inventory Toggle
		EnhancedInputComponent->BindAction(InventoryToggleAction, ETriggerEvent::Started, this, &AMegaJamCharacter::InventoryToggle);

		// Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMegaJamCharacter::PlayerInteract);
	}
	else
	{
		UE_LOG(LogMegaJam, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMegaJamCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMegaJamCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMegaJamCharacter::Sprint()
{
	CurrentMovementType = EMovementType::EMT_Sprinting;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AMegaJamCharacter::EndSprint()
{
	CurrentMovementType = EMovementType::EMT_Walking;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMegaJamCharacter::StartCrouch()
{
	CurrentMovementType = EMovementType::EMT_Crouching;
	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
}

void AMegaJamCharacter::EndCrouch()
{
	CurrentMovementType = EMovementType::EMT_Walking;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMegaJamCharacter::UsePrimary()
{
	if (!PrimaryHand) return;
	if (PrimaryHand->Implements<UInteractable>())
	{
		IInteractable::Execute_ItemInteract(PrimaryHand);
	}
}

void AMegaJamCharacter::Reload()
{
	if (CurrentPrimaryType == EPrimaryType::EPT_Ranged)
	{
		//TODO: increase ammo count up to max clip size or total ammo count whichever is lower
		//TODO: play reload animation
	}
}

void AMegaJamCharacter::UseQuickItem()
{
	if(!QuickSlotItem) return;
	if (QuickSlotItem->Implements<UInteractable>())
	{
		IInteractable::Execute_ItemInteract(QuickSlotItem);
	}
}

void AMegaJamCharacter::InventoryToggle()
{
	//TODO : open inventory UI | Probably to be done in Blueprint
}

void AMegaJamCharacter::PlayerInteract()
{
	CurrentInteractableFocus = GetNearestInteractableActor();
	if (CurrentInteractableFocus)
	{
		UE_LOG(LogMegaJam, Warning, TEXT("Interacting with %s"), *CurrentInteractableFocus->GetName());
		//IInteractable* Interactable = Cast<IInteractable>(CurrentInteractableFocus);
		if (CurrentInteractableFocus->Implements<UInteractable>())
		{
			IInteractable::Execute_ItemInteract(CurrentInteractableFocus);
		}
		else 
		{
			UE_LOG(LogMegaJam, Warning, TEXT("No Interactable"));
		}
	}
	else
	{
		UE_LOG(LogMegaJam, Warning, TEXT("No Current Interactable Focus"));
	}
}

void AMegaJamCharacter::StartAim()
{
	//TODO: Leave in blueprint for now?
}

void AMegaJamCharacter::EndAim()
{
}

void AMegaJamCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMegaJamCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMegaJamCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AMegaJamCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AMegaJamCharacter::OnInteractSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogMegaJam, Warning, TEXT("Overlap Begin with %s"), *OtherActor->GetName());
	IInteractable* Interactable = Cast<IInteractable>(OtherActor);
	if (Interactable)
	{
		InteractableActors.AddUnique(OtherActor);
		if (!CurrentInteractableFocus)
		{
			CurrentInteractableFocus = OtherActor;
		}
	}
	else
	{
		UE_LOG(LogMegaJam, Warning, TEXT("Cast Failed"));
	}
}

void AMegaJamCharacter::OnInteractSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractable* Interactable = Cast<IInteractable>(OtherActor);
	if (Interactable)
	{
		InteractableActors.Remove(OtherActor);
	}
}

AActor* AMegaJamCharacter::GetNearestInteractableActor()
{
	if(InteractableActors.Num() == 0) return nullptr;
	float NearestDistance = (CurrentInteractableFocus->GetActorLocation() - GetActorLocation()).Size();
	AActor* NewFocus = nullptr;
	if (CurrentInteractableFocus)
	{
		NewFocus = CurrentInteractableFocus;
	}
	for (AActor* Actor : InteractableActors)
	{ 
		
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() < NearestDistance)
		{
			NearestDistance = (Actor->GetActorLocation() - GetActorLocation()).Size();
			NewFocus = Actor;
		}
	}
	if (NewFocus)
	{
		return NewFocus;
	}
	return nullptr;
}
