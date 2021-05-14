﻿#include "UI/ProjectCleanerCorruptedFilesUI.h"
// Engine Headers
#include "IContentBrowserSingleton.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Engine/World.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SUserWidget.h"

#define LOCTEXT_NAMESPACE "FProjectCleanerModule"

void SProjectCleanerCorruptedFilesUI::Construct(const FArguments& InArgs)
{
	SetCorruptedFiles(InArgs._CorruptedFiles);
	RefreshUIContent();
}

void SProjectCleanerCorruptedFilesUI::RefreshUIContent()
{
	FAssetPickerConfig Config;
	Config.InitialAssetViewType = EAssetViewType::List;
	Config.bAddFilterUI = true;
	Config.bShowPathInColumnView = true;
	Config.bSortByPathInColumnView = true;
	Config.bShowBottomToolbar = true;
	Config.bCanShowDevelopersFolder = false;
	Config.bAllowDragging = false;
	Config.AssetShowWarningText = FText::FromName("No assets");

	FARFilter Filter;
	if(CorruptedFiles.Num() == 0)
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
	for(const auto& Asset : CorruptedFiles)
	{
		Filter.PackageNames.Add(Asset.PackageName);
	}
	Config.Filter = Filter;
	FContentBrowserModule& ContentBrowser = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	
	WidgetRef = SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.Padding(FMargin{40.0f})
	.AutoHeight()
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.AutoWrapText(true)
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Light.ttf"),20))
			.Text(LOCTEXT("corruptedfiles", "Assets below are failed to load and possibly corrupted"))
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin{0.0f, 10.0f})
		[
			SNew(STextBlock)
			.AutoWrapText(true)
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Light.ttf"),10))
			.Text(LOCTEXT("corruptedfilesfixtext", "To fix them:\n\t1.Close Editor\n\t2.Delete that files manually from Windows explorer"))
		]
	]
	+ SVerticalBox::Slot()
	.Padding(FMargin{40.0f})
	.AutoHeight()
	[
		ContentBrowser.Get().CreateAssetPicker(Config)
	];
	
	ChildSlot
	[
		WidgetRef
	];
}

void SProjectCleanerCorruptedFilesUI::SetCorruptedFiles(const TArray<FAssetData>& NewCorruptedFiles)
{
	if (NewCorruptedFiles.Num() == 0) return;

	for(const auto& File : NewCorruptedFiles)
	{
		CorruptedFiles.Add(File);
	}
	
	RefreshUIContent();
}

#undef LOCTEXT_NAMESPACE
