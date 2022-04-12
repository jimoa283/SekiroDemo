// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/RPGInventoryInterface.h"
#include "InventoryComponent.generated.h"

class URPGTokenItem;
class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTokensNumChange,int32,BeforeValue,int32,DeltaValue);

UCLASS(ClassGroup=(Custom),meta=(BlueprintSpawnableComponent))
class SERIKODEMO_API UInventoryComponent : public UActorComponent,public IRPGInventoryInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	APlayerCharacter* PlayerCharacter;

public: 

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Inventory)
	TMap<URPGItem*,FRPGItemData> UseItemMap;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Inventory)
	TMap<URPGItem*,FRPGItemData> SkillItemMap;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Inventory)
	TMap<URPGItem*,FRPGItemData> ArtificialHandMap;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Inventory)
	TMap<URPGItem*,FRPGItemData> TokensMap;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Inventory)
	TMap<FRPGItemSlot, URPGItem*> EquipmentUseItemList;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Inventory)
	TMap<FRPGItemSlot, URPGItem*>  EquipmentSkillList;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Inventory)
	TMap<FRPGItemSlot, URPGItem*>  EquipmentArtificialHandList;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Inventory)
	FRPGItemSlot CurrentEquipUseItemSlot;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Inventory)
	FRPGItemSlot CurrentEquipSkillSlot;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Inventory)
	FRPGItemSlot CurrentEquipAritficialHandSlot;

	/* Delegate called when an inventory item has been added or removed */
	UPROPERTY(BlueprintAssignable,Category=Inventory)
	FOnInventoryItemChanged OnInventoryItemChanged;

	/* Native version above, called before BP delegate */
	FOnInventoryItemChangedNative OnInventoryItemChangedNative;

	/* Delegate called when an inventory slot has changed */
	UPROPERTY(BlueprintAssignable,Category=Inventory)
	FOnSlottedItemChanged OnSlottedItemChanged;

	/* Native version above,called before BP delegate */
	FOnSlottedItemChangedNative OnSlottedItemChangedNative;

	/* Delegate called when the inventory has been load/reload */
	UPROPERTY(BlueprintAssignable,Category=Inventory)
	FOnInventoryLoaded OnInventoryLoaded;

	UPROPERTY(BlueprintAssignable,Category=Inventory)
	FOnSlottedItemChanged OnCurrentUseItemSlotChanged;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnSlottedItemChanged OnCurrentArtificialHandItemSlotChanged;

	/* Native version above, called before BP delegate */
	FOnInventoryLoadedNative OnInventoryLoadedNative;

	UPROPERTY(BlueprintAssignable)
	FOnTokensNumChange OnMoneyNumChange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Money)
	URPGTokenItem* MoneyItem;

	/* Called after the inventory was changed and we notified all delegates */
	UFUNCTION(BlueprintCallable,Category=Inventory)
	void InventoryItemChanged(bool bAdded,URPGItem* Item);

	/* Called after an item was equipped and we notified all delegates */
	UFUNCTION(BlueprintCallable,Category=Inventory)
	void SlottedItemChanged(FRPGItemSlot ItemSlot,URPGItem* Item);


	/* Adds a new inventory item, will add it to an empty slot if possible, If the item supports count you can add more than one count, It will also update the level when adding if required */
	UFUNCTION(BlueprintCallable,Category=Inventory)
	bool AddInventoryItem(URPGItem* NewItem,int32 ItemCount=1,int32 ItemLevel=1,bool bAutoSlot=true);

	/* Remove an inventory item, will also remove from slots,A remove cout of<=0 means to remove all copies */
	UFUNCTION(BlueprintCallable,Category=Inventory)
	bool RemoveInventoryItem(URPGItem* RemovedItem,int32 RemoveCount=1);

	/* Returns all inventory items of a given type. If none is passed as type it will return all */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void GetInventoryItems(TArray<URPGItem*>& Items, FPrimaryAssetType ItemType);

	/* Returns all inventory items */
	UFUNCTION(BlueprintPure,Category=Inventory)
	int32 GetInventoryItemCount(URPGItem* Item);

	/* Returns the item data associated with an item. Returns false if none found */
	UFUNCTION(BlueprintPure,Category=Inventory)
	bool GetInventoryItemData(URPGItem* Item,FRPGItemData& ItemData);

	/* Sets slot to item, will remove from other slots if necessary. If passing null this will empty the slot */
	UFUNCTION(BlueprintCallable,Category=Inventory)
	bool SetSlottedItem(const FRPGItemSlot& ItemSlot,URPGItem* Item);

	/* Returns item in slot, or null if empty */
	UFUNCTION(BlueprintPure,Category=Inventory)
	URPGItem* GetSlottedItem(const FRPGItemSlot& ItemSlot);

	/* Returns all slotted items of a given type. If none is passed as type it will return all */
	UFUNCTION(BlueprintCallable,Category=Inventory)
	void GetSlottedItems(TArray<URPGItem*>& Items,FPrimaryAssetType ItemType,bool bOutputEmptyIndexes);

	UFUNCTION(BlueprintCallable,Category=Inventory)
	void AddMoney(int32 DeltaValue);

	UFUNCTION(BlueprintCallable,Category=Inventory)
	int32 GetMoneyCount();

	UFUNCTION(BlueprintCallable,Category=Inventory)
	bool LoadInventory();

	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() override
	{
		return OnInventoryItemChangedNative;
	}

	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() override
	{
		return OnSlottedItemChangedNative;
	}

protected:

   /* Calls the inventory update callbacks */
   void NotifyInventoryItemChanged(bool bAdded,URPGItem* Item);
   void NotifySlottedItemChanged(FRPGItemSlot ItemSlot,URPGItem* Item);
   void NotifyInventoryLoaded();
   void NotifyCurrentEquipItemSlotChanged(FRPGItemSlot ItemSlot,URPGItem* Item);

   TMap<URPGItem*, FRPGItemData>* GetItemMapByItemType(const FPrimaryAssetType& ItemType);

   TMap<FRPGItemSlot, URPGItem*>* GetEquipmentListByItemType(const FPrimaryAssetType& ItemType);

   FRPGItemSlot* GetCurrentEquipItemByItemType(const FPrimaryAssetType& ItemType);

public:
   bool CanUseCurrentItem();

   void ChangeToOtherEquipItem(const FPrimaryAssetType& ItemType,bool IsUp);

};
