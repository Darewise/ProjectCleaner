// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StructsContainer.h"
#include "UI/SProjectCleanerBrowser.h"
#include "UI/ProjectCleanerBrowserStatisticsUI.h"
// Engine Headers
#include "Input/Reply.h"
#include "Modules/ModuleInterface.h"
#include "ContentBrowserDelegates.h"
#include "CoreMinimal.h"

class ProjectCleanerNotificationManager;
class FToolBarBuilder;
class STableViewBase;
class FMenuBuilder;
class ITableRow;
class SDockTab;

struct FSlateColorBrush;
struct FAssetData;
struct FSlateBrush;

DECLARE_LOG_CATEGORY_EXTERN(LogProjectCleaner, Log, All);

class FProjectCleanerModule : public IModuleInterface
{
public:
	FProjectCleanerModule()
	{
		NotificationManager = nullptr;
		DirectoryFilterSettings = nullptr;
	}
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	
	/**
	 * @brief Opens ProjectCleanerBrowser Main Tab
	 */
	void PluginButtonClicked();
	
private:
	void InitModuleComponents();
	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);
	TSharedRef<SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	FReply RefreshBrowser();
	void UpdateStats();
	void InitCleaner();
	void Reset();
	/**
	 * Sets content browser focus to root directory and refreshes asset registry
	 * @brief Updates content browser
	 */
	void UpdateContentBrowser() const;

	FString GetStringValueForCustomColumn(FAssetData& AssetData, FName ColumnName) const;
	FText GetDisplayTextForCustomColumn(FAssetData& AssetData, FName ColumnName) const;

	// Button events Start //
	FReply OnDeleteEmptyFolderClick();
	FReply OnDeleteUnusedAssetsBtnClick();
	// Button events End //

	/**
	 * @brief Creates confirmation window with yes/no options
	 * @param Title - Window Title
	 * @param ContentText - Window content text
	 * @return EAppReturnType::Type Window confirmation status
	 */
	EAppReturnType::Type ShowConfirmationWindow(const FText& Title, const FText& ContentText) const;

	/**
	 * @brief Checks if confirmation window cancelled or not
	 * @param Status EAppReturnType::Type
	 * @return bool
	 */
	static bool IsConfirmationWindowCanceled(EAppReturnType::Type Status);

	FCleaningStats CleaningStats;
	FStandardCleanerText StandardCleanerText;
	TSharedPtr<class FUICommandList> PluginCommands;
	TArray<FAssetData> UnusedAssets;
	TArray<FNode> AdjacencyList;
	TArray<FString> EmptyFolders;
	TArray<FString> NonProjectFiles;
	TArray<FSourceCodeFile> SourceFiles;
	ProjectCleanerNotificationManager* NotificationManager;
	TWeakPtr<SProjectCleanerBrowser> ProjectCleanerBrowserUI;
	UDirectoryFilterSettings* DirectoryFilterSettings;
	UNonUProjectFiles* NonUProjectFilesSettings;
	UUnusedAssetsUIContainer* UnusedAssetsUIContainerSettings;

	TSharedPtr<SWidget> OnGetAssetContextMenu(const TArray<FAssetData>& SelectedAssets);
	void FindInContentBrowser() const;

	// UI
	TWeakPtr<SProjectCleanerBrowserStatisticsUI> ProjectCleanerBrowserStatisticsUI;
};


