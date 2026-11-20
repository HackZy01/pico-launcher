#include "common.h"
#include <string.h>
#include <algorithm>
#include "FileType/Nds/NdsFileType.h"
#include "GameEdition.h"
#include "SdFolder.h"

SdFolder::SdFolder(FileInfo** files, int fileCount)
    : _files(files), _fileCount(fileCount) { }

SdFolder::~SdFolder()
{
    for (int i = 0; i < _fileCount; i++)
        delete _files[i];
    free(_files);
}

std::unique_ptr<const FileInfo*[]> SdFolder::FilterAndSort(
    const SdFolderFilterSortParams& filterSortParams, u32& resultCount) const
{
    auto sortedFilteredFiles = std::make_unique<const FileInfo*[]>(_fileCount);
    u32 filteredCount = 0;
    for (int i = 0; i < _fileCount; i++)
    {
        const FileInfo* file = _files[i];
        bool isHidden = file->GetFileName()[0] == '.' || file->IsHidden();
        auto classification = file->GetFileType()->GetClassification();
        if (classification != FileTypeClassification::Unknown &&
            !file->IsEditionVariant() &&
            (!isHidden || filterSortParams.includeHiddenFiles))
        {
            sortedFilteredFiles[filteredCount++] = file;
        }
    }
    std::sort(sortedFilteredFiles.get(), sortedFilteredFiles.get() + filteredCount,
        [filterSortParams] (const FileInfo*& a, const FileInfo*& b)
        {
            bool result = true;
            if (CompareClassification(a, b, result))
                return result;

            auto sortType = filterSortParams.sortType;
            auto sortDirection = filterSortParams.sortDirection;
            if (a->GetFileType()->GetClassification() == FileTypeClassification::Folder &&
                b->GetFileType()->GetClassification() == FileTypeClassification::Folder)
            {
                if (sortType != SdFolderSortType::Name)
                {
                    sortType = SdFolderSortType::Name;
                    sortDirection = SdFolderSortDirection::Ascending;
                }
            }
            switch (sortType)
            {
                case SdFolderSortType::Name:
                default:
                {
                    result = CompareName(a, b);
                    break;
                }
            }
            return sortDirection == SdFolderSortDirection::Ascending ? result : !result;
        });
    resultCount = filteredCount;
    return sortedFilteredFiles;
}

bool SdFolder::CompareClassification(const FileInfo* a, const FileInfo* b, bool& result)
{
    auto aClassification = a->GetFileType()->GetClassification();
    auto bClassification = b->GetFileType()->GetClassification();
    if (aClassification == bClassification)
        return false;

    if (aClassification == FileTypeClassification::Folder)
    {
        result = true;
        return true;
    }
    else if (bClassification == FileTypeClassification::Folder)
    {
        result = false;
        return true;
    }

    return false;
}

bool SdFolder::CompareName(const FileInfo* a, const FileInfo* b)
{
    return strcasecmp(a->GetFileName(), b->GetFileName()) < 0;
}

void SdFolder::LinkEditionVariants()
{
    if (_fileCount < 2)
        return;

    // sorted view of the files so that base files can be found with a binary search
    auto sorted = std::make_unique_for_overwrite<FileInfo*[]>(_fileCount);
    memcpy(sorted.get(), _files, sizeof(FileInfo*) * _fileCount);
    std::sort(sorted.get(), sorted.get() + _fileCount,
        [] (const FileInfo* a, const FileInfo* b) { return CompareName(a, b); });

    const int suffixLength = strlen(EDITION_VARIANT_SUFFIX);
    for (int i = 0; i < _fileCount; i++)
    {
        FileInfo* variant = _files[i];
        if (variant->GetFileType() != &NdsFileType::sInstance)
            continue;

        // split "Name_pl.nds" into the stem "Name_pl" and the extension ".nds"
        const char* name = variant->GetFileName();
        const char* extension = strrchr(name, '.');
        if (!extension)
            continue;

        const int stemLength = extension - name;
        if (stemLength <= suffixLength ||
            strncasecmp(name + stemLength - suffixLength, EDITION_VARIANT_SUFFIX, suffixLength) != 0)
        {
            continue;
        }

        // build the name of the base file: "Name.nds"
        const int baseStemLength = stemLength - suffixLength;
        const int extensionLength = strlen(extension);
        auto baseName = std::make_unique_for_overwrite<char[]>(baseStemLength + extensionLength + 1);
        memcpy(baseName.get(), name, baseStemLength);
        memcpy(baseName.get() + baseStemLength, extension, extensionLength + 1);

        FileInfo** end = sorted.get() + _fileCount;
        FileInfo** match = std::lower_bound(sorted.get(), end, baseName.get(),
            [] (const FileInfo* entry, const char* value)
            {
                return strcasecmp(entry->GetFileName(), value) < 0;
            });
        if (match == end || strcasecmp((*match)->GetFileName(), baseName.get()) != 0)
            continue; // no base file, so the _pl file is just a regular game

        FileInfo* base = *match;
        if (base->GetFileType() != &NdsFileType::sInstance || base->HasEditionVariant())
            continue;

        base->SetEditionVariant(variant);
        variant->SetIsEditionVariant(true);
    }
}

void SdFolder::SortByNameInPlace()
{
    std::sort(_files, _files + _fileCount, CompareName);
}

const FileInfo* SdFolder::BinarySearch(const char* fileName) const
{
    if (_fileCount != 0)
    {
        const auto file = std::lower_bound(_files, _files + _fileCount, fileName,
            [] (const FileInfo* entry, const char* value)
            {
                return strcasecmp(entry->GetFileName(), value) < 0;
            });

        if (file != _files + _fileCount && !strcasecmp((*file)->GetFileName(), fileName))
        {
            return *file;
        }
    }

    return nullptr;
}