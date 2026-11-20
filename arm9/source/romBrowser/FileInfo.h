#pragma once
#include "fat/ff.h"
#include "fat/FastFileRef.h"
#include "FileType/FileType.h"

class InternalFileInfo;

class FileInfo
{
public:
    FileInfo() { }
    FileInfo(const FileInfo& fileInfo);
    FileInfo(const TCHAR* fileName, const FileType* type, const FastFileRef& fastFileRef, u8 attributes);

    FileInfo &operator=(FileInfo&& rhs)
    {
        if (this != &rhs)
        {
            _name = std::move(rhs._name);
            _type = rhs._type;
            _fastFileRef = rhs._fastFileRef;
            _editionVariant = nullptr;
            _isEditionVariant = false;
        }

        return *this;
    }

    const TCHAR* GetFileName() const { return _name.get(); }
    const FileType* GetFileType() const { return _type; }
    u32 GetFileSize() const { return _fastFileRef.GetFileSize(); }

    InternalFileInfo* CreateInternalFileInfo() const
    {
        return _type->CreateInternalFileInfo(_fastFileRef);
    }

    const FastFileRef& GetFastFileRef() const { return _fastFileRef; }

    bool IsReadOnly() const { return _attributes & AM_RDO; }
    bool IsHidden() const { return _attributes & AM_HID; }
    bool IsSystem() const { return _attributes & AM_SYS; }

    /// @brief Whether this file has an edition variant (e.g. "Game_pl.nds" for "Game.nds").
    bool HasEditionVariant() const { return _editionVariant != nullptr; }

    /// @brief Whether this file is the variant edition of another file in the same folder.
    ///        Such files are hidden from the browser and reached through their base file.
    bool IsEditionVariant() const { return _isEditionVariant; }

    const FileInfo* GetEditionVariant() const { return _editionVariant; }
    void SetEditionVariant(const FileInfo* variant) { _editionVariant = variant; }
    void SetIsEditionVariant(bool isVariant) { _isEditionVariant = isVariant; }

private:
    std::unique_ptr<TCHAR[]> _name;
    const FileType* _type;
    FastFileRef _fastFileRef;
    u8 _attributes;
    const FileInfo* _editionVariant = nullptr; // not owned, points to another FileInfo in the same SdFolder
    bool _isEditionVariant = false;
};