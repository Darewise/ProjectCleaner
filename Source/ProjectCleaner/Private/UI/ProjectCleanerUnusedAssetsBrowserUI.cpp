﻿#include "UI/ProjectCleanerUnusedAssetsBrowserUI.h"
// Engine Headers
#include "IContentBrowserSingleton.h"
#include "ProjectCleaner.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Toolkits/GlobalEditorCommonCommands.h"
#include "UI/ProjectCleanerBrowserCommands.h"

#define LOCTEXT_NAMESPACE "FProjectCleanerModule"

void SProjectCleanerUnusedAssetsBrowserUI::Construct(const FArguments& InArgs)
{
	UnusedAssets = InArgs._UnusedAssets;

	FProjectCleanerBrowserCommands::Register();
	
	FDetailsViewArgs UnusedAssetsUISettingsDetailsViewArgs;
	UnusedAssetsUISettingsDetailsViewArgs.ViewIdentifier = "UnusedAssetsUISettings";

	Commands = MakeShareable(new FUICommandList);
	Commands->MapAction(
		FGlobalEditorCommonCommands::Get().FindInContentBrowser,
		FUIAction(
			FExecuteAction::CreateRaw(this, &SProjectCleanerUnusedAssetsBrowserUI::FindInContentBrowser),
			FCanExecuteAction::CreateRaw(this, &SProjectCleanerUnusedAssetsBrowserUI::IsAnythingSelected)
		)
	);
	Commands->MapAction(
		FProjectCleanerBrowserCommands::Get().ViewReferences,
		FUIAction()
	);	

	RefreshUIContent();
	
	ChildSlot
	[
		WidgetRef
	];
}

void SProjectCleanerUnusedAssetsBrowserUI::SetUnusedAssets(TArray<FAssetData*>& NewUnusedAssets)
{
	UnusedAssets = NewUnusedAssets;
	RefreshUIContent();
}

TSharedPtr<SWidget> SProjectCleanerUnusedAssetsBrowserUI::OnGetAssetContextMenu(
	const TArray<FAssetData>& SelectedAssets)
{
	FMenuBuilder MenuBuilder{true, Commands};
	MenuBuilder.BeginSection(
		TEXT("Asset"),
		NSLOCTEXT("ReferenceViewerSchema", "AssetSectionLabel", "Asset")
	);
	{
		MenuBuilder.AddMenuEntry(FGlobalEditorCommonCommands::Get().FindInContentBrowser);
	}
	MenuBuilder.EndSection();
	MenuBuilder.BeginSection(
		TEXT("References"),
		NSLOCTEXT("ReferenceViewerSchema", "ReferencesSectionLabel", "References")
	);
	{
		MenuBuilder.AddMenuEntry(FProjectCleanerBrowserCommands::Get().ViewReferences);
	}
	MenuBuilder.EndSection();


	return MenuBuilder.MakeWidget();
}

void SProjectCleanerUnusedAssetsBrowserUI::FindInContentBrowser() const
{
	if (!GetCurrentSelectionDelegate.IsBound()) return;

	const TArray<FAssetData> CurrentSelection = GetCurrentSelectionDelegate.Execute();
	if (CurrentSelection.Num() > 0)
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(
			"ContentBrowser");
		ContentBrowserModule.Get().SyncBrowserToAssets(CurrentSelection);
	}
}

bool SProjectCleanerUnusedAssetsBrowserUI::IsAnythingSelected() const
{
	if (!GetCurrentSelectionDelegate.IsBound()) return false;

	const TArray<FAssetData> CurrentSelection = GetCurrentSelectionDelegate.Execute();
	return CurrentSelection.Num() > 0;
}

void SProjectCleanerUnusedAssetsBrowserUI::RefreshUIContent()
{
	FAssetPickerConfig Config;
	Config.InitialAssetViewType = EAssetViewType::Tile;
	Config.bAddFilterUI = true;
	Config.bShowPathInColumnView = true;
	Config.bSortByPathInColumnView = true;
	Config.bForceShowEngineContent = false;
	Config.bShowBottomToolbar = true;
	Config.bCanShowDevelopersFolder = false;
	Config.bAllowDragging = false;	
	Config.AssetShowWarningText = FText::FromName("No assets");
	Config.GetCurrentSelectionDelegates.Add(&GetCurrentSelectionDelegate);
	Config.OnGetAssetContextMenu = FOnGetAssetContextMenu::CreateRaw(
		this,
		&SProjectCleanerUnusedAssetsBrowserUI::OnGetAssetContextMenu
	);


	FARFilter Filter;
	if(UnusedAssets.Num() == 0)
	{
		// this is needed when there is no assets to show ,
		// asset picker will show remaining assets in content browser,
		// we must not show them
		Filter.TagsAndValues.Add(FName{"ProjectCleanerEmptyTag"}, FString{"ProjectCleanerEmptyTag"});
	}
	else
	{
		// excluding level assets from showing and filtering
		Filter.bRecursiveClasses = true;
		Filter.RecursiveClassesExclusionSet.Add(UWorld::StaticClass()->GetFName());
	}
	
	for(const auto& Asset : UnusedAssets)
	{
		Filter.PackageNames.Add(Asset->PackageName);
	}
	Config.Filter = Filter;
	UE_LOG(LogProjectCleaner, Warning, TEXT("Refreshing UI content"));

	FContentBrowserModule& ContentBrowser = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	WidgetRef = SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(20.0f)
	[
		SNew(STextBlock)
		.AutoWrapText(true)
		.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Light.ttf"),20))
		.Text(LOCTEXT("UnusedAssets", "All Unused Assets"))
	]
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(20.0f)
	[
		ContentBrowser.Get().CreateAssetPicker(Config)
	];
	
	ChildSlot
	[
		WidgetRef
	];
}

#undef LOCTEXT_NAMESPACE
