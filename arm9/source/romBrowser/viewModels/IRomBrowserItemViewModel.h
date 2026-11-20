#pragma once
#include "core/task/TaskQueue.h"

class IRomBrowserItemViewModel
{
public:
    virtual ~IRomBrowserItemViewModel() = default;

    virtual void Activate() = 0;
    virtual void ShowGameInfo() = 0;

    /// @brief Selects the original or variant edition of the item (see GameEdition.h).
    /// @param useVariant True to select the variant edition, or false to select the original.
    /// @return True when the item has an edition variant (and the input was thus handled), or false otherwise.
    virtual bool SelectEdition(bool useVariant) { return false; }
    virtual void SetIndex(int index) = 0;
    virtual void SetQueueTask(QueueTask<void> queueTask) = 0;
    virtual void CancelQueueTask() = 0;
    virtual void DisposeQueueTaskWhenComplete() = 0;

protected:
    IRomBrowserItemViewModel() = default;
};