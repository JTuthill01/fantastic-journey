#pragma once

UENUM(BlueprintType)
enum class EInteractableType : uint8 { EIT_None UMETA(DisplayName = "None"), EIT_Door UMETA(DisplayName = "Door"),
EIT_Container UMETA(DisplayName = "Container"), EIT_Pickup UMETA(DisplayName = "Pickup") }; 