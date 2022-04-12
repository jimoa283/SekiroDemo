// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/InventoryComponent.h"
#include "Item/RPGItem.h"
#include "Item/RPGTokenItem.h"
#include "RPGGameInstance.h"
#include "Character/PlayerCharacter.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{


}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter=GetOwner<APlayerCharacter>();

	LoadInventory();
	//PlayerCharacter->InitInventory();
}


void UInventoryComponent::InventoryItemChanged(bool bAdded, URPGItem* Item)
{
   
}

void UInventoryComponent::SlottedItemChanged(FRPGItemSlot ItemSlot, URPGItem* Item)
{
   
}

bool UInventoryComponent::AddInventoryItem(URPGItem* NewItem, int32 ItemCount/*=1*/, int32 ItemLevel/*=1*/, bool bAutoSlot/*=true*/)
{
   bool bChanged=false;
   if (!NewItem)
   {
	   UE_LOG(LogTemp,Warning,TEXT("AddInventoryItem: Failed trying to add null item!"));
	   return false;
   }

   if (ItemCount <= 0 || ItemLevel <= 0)
   {
	   UE_LOG(LogTemp, Warning, TEXT("AddInventoryItem: Failed trying to add item %s with negative count or level!"), *NewItem->GetName());
	   return false;
   }

   TMap<URPGItem*,FRPGItemData>* _ItemMap=GetItemMapByItemType(NewItem->ItemType);
   if(!_ItemMap)
      return false;

	TMap<URPGItem*,FRPGItemData> ItemMap=*_ItemMap;

   //Find current item data,which may be empty
   FRPGItemData OldData;
   GetInventoryItemData(NewItem,OldData);   //获取在背包中该种类物品的数据

   //Find modiffied data
   FRPGItemData NewData=OldData;
   NewData.UpdateItemData(FRPGItemData(ItemCount,ItemLevel),NewItem->MaxCount,NewItem->MaxLevel);

   if (OldData != NewData)
   {
	   // If data changed, need to update storage and call callback
	  ItemMap.Add(NewItem,NewData);
	  NotifyInventoryItemChanged(true,NewItem);
	  bChanged=true;
   }

   if (bChanged)
   {
	   // If anything changed,write to save game
	   return true;
   }

   return false;
}

bool UInventoryComponent::RemoveInventoryItem(URPGItem* RemovedItem, int32 RemoveCount/*=1*/)
{
	if (!RemovedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveInventoryItem: Failed trying to remove null item!"));
		return false;
    }

	TMap<URPGItem*, FRPGItemData>& ItemMap=*GetItemMapByItemType(RemovedItem->ItemType);

	// Find current item data,which may be empty
	FRPGItemData NewData;
	GetInventoryItemData(RemovedItem,NewData);

	if (!NewData.IsValid())
	{
		// Wasn't found
		return false;
	}

    // If RemoveCount <=0 ,delete all
	if (RemoveCount <= 0)
	{
		NewData.ItemCount=0;
	}
	else
	{
		NewData.ItemCount-=RemoveCount;
	}

	if (NewData.ItemCount > 0||RemovedItem->bCantBeMove)
	{
        // Update data with new count
		//InventoryData.Add(RemovedItem,NewData);
		ItemMap[RemovedItem]=NewData;

		TMap<FRPGItemSlot, URPGItem*>& EquipmentList = *GetEquipmentListByItemType(RemovedItem->ItemType);

		for (TPair<FRPGItemSlot, URPGItem*>& Pair : EquipmentList)
		{
			if (Pair.Value == RemovedItem)
			{
			    if(Pair.Key==*GetCurrentEquipItemByItemType(RemovedItem->ItemType))
				   NotifyCurrentEquipItemSlotChanged(Pair.Key,Pair.Value);

				NotifySlottedItemChanged(Pair.Key, Pair.Value);
				break;
			}
		}

	}
	else 
	{
		//Remove item entirely, make sure it is unslotted
		ItemMap.Remove(RemovedItem);

		TMap<FRPGItemSlot,URPGItem*>& EquipmentList=*GetEquipmentListByItemType(RemovedItem->ItemType);

		for (TPair<FRPGItemSlot, URPGItem*>& Pair : EquipmentList)
		{
			if (Pair.Value == RemovedItem)
			{
				Pair.Value=nullptr;

				ChangeToOtherEquipItem(RemovedItem->ItemType,false);;

				NotifySlottedItemChanged(Pair.Key,Pair.Value);
				break;
			}
		}
	}

	// If we got this far, there is a change so notify and save
	NotifyInventoryItemChanged(false,RemovedItem);

	return true;

}

void UInventoryComponent::GetInventoryItems(TArray<URPGItem*>& Items, FPrimaryAssetType ItemType)
{
	TMap<URPGItem*, FRPGItemData>* _ItemMap=GetItemMapByItemType(ItemType);

	if (!_ItemMap)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Wrong ItemMap");
		return;
	}

	TMap<URPGItem*, FRPGItemData>& ItemMap=*_ItemMap;

	for (const TPair<URPGItem*, FRPGItemData>& Pair :ItemMap)
	{
		if (Pair.Key)
		{
			FPrimaryAssetId AssetId=Pair.Key->GetPrimaryAssetId();

			// Filters based on item type
			if (AssetId.PrimaryAssetType == ItemType || !ItemType.IsValid())
			{
				Items.Add(Pair.Key);
			}
		}
   }
}

int32 UInventoryComponent::GetInventoryItemCount(URPGItem* Item)
{
	TMap<URPGItem*, FRPGItemData>* _ItemMap = GetItemMapByItemType(Item->ItemType);

	if (!_ItemMap)
		return false;

	TMap<URPGItem*, FRPGItemData>& ItemMap = *_ItemMap;

	FRPGItemData* FoundItem = ItemMap.Find(Item);

    if (FoundItem)
    {
	  return FoundItem->ItemCount;
    }

  return 0;
}

bool UInventoryComponent::GetInventoryItemData(URPGItem* Item, FRPGItemData& ItemData)
{
   TMap<URPGItem*, FRPGItemData>* _ItemMap= GetItemMapByItemType(Item->ItemType);

   if(!_ItemMap)
      return false;

    TMap<URPGItem*,FRPGItemData>& ItemMap=*_ItemMap;
	  
    const FRPGItemData* FoundItem=ItemMap.Find(Item);

  if (FoundItem)
  {
	  ItemData=*FoundItem;
	  return true;
  }

   ItemData=FRPGItemData(0,0);
   return false;
}

bool UInventoryComponent::SetSlottedItem(const FRPGItemSlot& ItemSlot, URPGItem* Item)
{
   // Iterate entire inventory because we need to remove from old slot
   bool bFound=false;

   TMap<FRPGItemSlot,URPGItem*>& EquipmentList=*GetEquipmentListByItemType(Item->ItemType);

   for (TPair<FRPGItemSlot, URPGItem*>& Pair : EquipmentList)
   {
	   if (Pair.Key == ItemSlot)
	   {
		   // Add to new slot
		   bFound=true;
		   Pair.Value=Item;

		   FRPGItemSlot& CurrentEquipSlot=*GetCurrentEquipItemByItemType(Item->ItemType);

		   if (!GetSlottedItem(CurrentEquipSlot))   //如果当前装备中的道具为空
		   {
			   CurrentEquipSlot=Pair.Key;          //则这个格子设为当前装备中的格子

			   NotifyCurrentEquipItemSlotChanged(CurrentEquipSlot,Item);
			   PlayerCharacter->ChangeEquipItem(Item,ItemSlot.ItemType);   
		   }
		   else if (CurrentEquipSlot==Pair.Key)     //如果当前格子为装备中的格子
		   {
			   NotifyCurrentEquipItemSlotChanged(CurrentEquipSlot, Item);
		      PlayerCharacter->ChangeEquipItem(Item,ItemSlot.ItemType);
		   }

		   NotifySlottedItemChanged(Pair.Key,Pair.Value);
	   }
	   else if (Item != nullptr && Pair.Value == Item)
	   {
		   // If this item was found in another slot, remove ite
		   Pair.Value=nullptr;
		   NotifySlottedItemChanged(Pair.Key,Pair.Value);
	   }
   }

   if (bFound)
   {
	   return true;
   }

   return false;
}

URPGItem* UInventoryComponent::GetSlottedItem(const FRPGItemSlot& ItemSlot)
{
   TMap<FRPGItemSlot, URPGItem*>& EquipmentList=*GetEquipmentListByItemType(ItemSlot.ItemType);
   
   URPGItem* const* FoundItem=EquipmentList.Find(ItemSlot);

   if (FoundItem)
   {
	   return *FoundItem;
   }

   return nullptr;
}

void UInventoryComponent::GetSlottedItems(TArray<URPGItem*>& Items, FPrimaryAssetType ItemType, bool bOutputEmptyIndexes)
{
   TMap<FRPGItemSlot,URPGItem*>& EquipmentList=*GetEquipmentListByItemType(ItemType);
    
	for (TPair<FRPGItemSlot, URPGItem*>& Pair : EquipmentList)
	{
		if (Pair.Key.ItemType == ItemType || !ItemType.IsValid())
		{
			Items.Add(Pair.Value);
		}
   }
}


void UInventoryComponent::AddMoney(int32 DeltaValue)
{
   int32 BeforeValue=GetMoneyCount();
   AddInventoryItem(MoneyItem,DeltaValue);
   OnMoneyNumChange.Broadcast(BeforeValue,DeltaValue);
}

int32 UInventoryComponent::GetMoneyCount()
{
   return GetInventoryItemCount(MoneyItem);
}

bool UInventoryComponent::LoadInventory()
{

   EquipmentUseItemList.Reset();
   EquipmentArtificialHandList.Reset();
   EquipmentSkillList.Reset();
   CurrentEquipUseItemSlot=FRPGItemSlot(URPGAssetManager::PotionItemType,0);
   CurrentEquipSkillSlot=FRPGItemSlot(URPGAssetManager::SkillItemType,0);
   CurrentEquipAritficialHandSlot=FRPGItemSlot(URPGAssetManager::WeaponItemType,0);

   UWorld* World=GetWorld();
   URPGGameInstance* GameInstance=World?World->GetGameInstance<URPGGameInstance>():nullptr;

   if(!GameInstance)
      return false;

   for (const TPair<FPrimaryAssetType, int32>& Pair : GameInstance->ItemSlotsPerType)
   {
       TMap<FRPGItemSlot,URPGItem*>& EquipmentList=*GetEquipmentListByItemType(Pair.Key);
	   for (int32 SlotNumber = 0; SlotNumber < Pair.Value; SlotNumber++)
	   {
		   EquipmentList.Add(FRPGItemSlot(Pair.Key,SlotNumber),nullptr);
	   }
   }

   return false;
}

void UInventoryComponent::NotifyInventoryItemChanged(bool bAdded, URPGItem* Item)
{
   // Notify native before blueprint
   OnInventoryItemChangedNative.Broadcast(bAdded,Item);
   OnInventoryItemChanged.Broadcast(bAdded,Item);

   // Call BP update event
   InventoryItemChanged(bAdded,Item);
}

void UInventoryComponent::NotifySlottedItemChanged(FRPGItemSlot ItemSlot, URPGItem* Item)
{
    // Notify native before blueprint
	OnSlottedItemChangedNative.Broadcast(ItemSlot,Item);
	OnSlottedItemChanged.Broadcast(ItemSlot,Item);

   // Call BP update event
   SlottedItemChanged(ItemSlot,Item);
}

void UInventoryComponent::NotifyInventoryLoaded()
{
   OnInventoryLoadedNative.Broadcast();
   OnInventoryLoaded.Broadcast();
}

void UInventoryComponent::NotifyCurrentEquipItemSlotChanged(FRPGItemSlot ItemSlot, URPGItem* Item)
{
	if (ItemSlot.ItemType == URPGAssetManager::PotionItemType)
	{
	   OnCurrentUseItemSlotChanged.Broadcast(ItemSlot,Item);
    }
	else if (ItemSlot.ItemType == URPGAssetManager::WeaponItemType)
	{
	   OnCurrentArtificialHandItemSlotChanged.Broadcast(ItemSlot,Item);
	}
}

TMap<URPGItem*, FRPGItemData>* UInventoryComponent::GetItemMapByItemType(const FPrimaryAssetType& ItemType)
{
	if(ItemType == URPGAssetManager::PotionItemType)
	    return &UseItemMap;
	else if(ItemType==URPGAssetManager::WeaponItemType)
	    return &ArtificialHandMap;
   else if(ItemType==URPGAssetManager::SkillItemType)
       return &SkillItemMap;
	else if(ItemType==URPGAssetManager::TokenItemType)
	   return &TokensMap;

	return nullptr;
}

TMap<FRPGItemSlot,URPGItem*>* UInventoryComponent::GetEquipmentListByItemType(const FPrimaryAssetType& ItemType)
{
	if (ItemType == URPGAssetManager::PotionItemType)
		return &EquipmentUseItemList;
	else if (ItemType == URPGAssetManager::WeaponItemType)
		return &EquipmentArtificialHandList;
	else if (ItemType == URPGAssetManager::SkillItemType)
		return &EquipmentSkillList;

	return nullptr;
}

FRPGItemSlot* UInventoryComponent::GetCurrentEquipItemByItemType(const FPrimaryAssetType& ItemType)
{
   if(ItemType==URPGAssetManager::PotionItemType)
     return &CurrentEquipUseItemSlot;
   else if(ItemType==URPGAssetManager::WeaponItemType)
     return &CurrentEquipAritficialHandSlot;
   else if(ItemType==URPGAssetManager::SkillItemType)
     return &CurrentEquipSkillSlot;

   return nullptr;
}

bool UInventoryComponent::CanUseCurrentItem() 
{
   return GetInventoryItemCount(GetSlottedItem(CurrentEquipUseItemSlot))>0;
}

void UInventoryComponent::ChangeToOtherEquipItem(const FPrimaryAssetType& ItemType, bool IsUp)
{
   bool HasEnd=false;

   TMap<FRPGItemSlot,URPGItem*>& EquipmentList=*GetEquipmentListByItemType(ItemType);

   FRPGItemSlot& CurrentEquipSlot =*GetCurrentEquipItemByItemType(ItemType);
   int32 CurrentIndex=CurrentEquipSlot.SlotNumber;
   int32 Length=EquipmentList.Num();

   while (!HasEnd)
   {
	   int32 OtherIndex=IsUp?(CurrentIndex-1+Length)%Length:(CurrentIndex+1)%Length;

	   FRPGItemSlot Temp=FRPGItemSlot(ItemType,OtherIndex);

	   if(Temp==CurrentEquipSlot)
	      return;
	   
	   if (!EquipmentList[Temp])
	   {
	      CurrentIndex=OtherIndex;
	   }
	   else
	   {
	      HasEnd=true;
		  
		  if(ItemType==URPGAssetManager::PotionItemType)
		    CurrentEquipUseItemSlot=Temp;
		  else if(ItemType==URPGAssetManager::WeaponItemType)
		    CurrentEquipAritficialHandSlot=Temp;
		  else if(ItemType==URPGAssetManager::SkillItemType)
		    CurrentEquipSkillSlot=Temp;

		  NotifyCurrentEquipItemSlotChanged(CurrentEquipSlot, EquipmentList[Temp]);
		  PlayerCharacter->ChangeEquipItem(EquipmentList[Temp],ItemType);

	   }
   }
}

