#pragma once

#include "forwards.h"
#include "guid.h"

#include <core/error.h>
#include <core/memory.h>
#include <core/shared_ptr.h>
#include <core/unique_ptr.h>
#include <core/page.h>
#include <core/hash_table.h>
#include <core/rwsem.h>
#include <core/list.h>
#include <core/thread.h>
#include <core/runnable.h>
#include <core/event.h>
#include <core/bio.h>
#include <core/ring_buffer.h>

namespace KStor
{

class Journal;

typedef Core::SharedPtr<Api::JournalTxBlock, Core::Memory::PoolType::Kernel> JournalTxBlockPtr;

class Transaction
{
friend Journal;
public:
    Transaction(Journal& journal, Core::Error &err);

    virtual ~Transaction();

    Core::Error Write(const Core::PageInterface& page, uint64_t position);

    const Guid& GetTxId() const;

    Core::Error Commit();

    void Cancel();

private:

    void OnCommitCompleteLocked(const Core::Error& result);
    void OnCommitComplete(const Core::Error& result);
    JournalTxBlockPtr CreateTxBlock(unsigned int type);

    Core::Error WriteTx(Core::NoIOBioList& bioList);

    Journal& JournalRef;
    unsigned int State;
    Guid TxId;
    JournalTxBlockPtr BeginBlock;
    Core::LinkedList<JournalTxBlockPtr, Core::Memory::PoolType::Kernel> DataBlockList;
    JournalTxBlockPtr CommitBlock;
    Core::RWSem Lock;
    Core::Event CommitEvent;
    Core::Error CommitResult;
};

typedef Core::SharedPtr<Transaction, Core::Memory::PoolType::Kernel> TransactionPtr;

const unsigned int JournalStateNew = 1;
const unsigned int JournalStateReplaying = 2;
const unsigned int JournalStateRunning = 3;
const unsigned int JournalStateStopping = 4;
const unsigned int JournalStateStopped = 4;

class Journal : public Core::Runnable
{

friend Transaction;

public:
    Journal(Volume& volume);

    Core::Error Load(uint64_t start);
    Core::Error Format(uint64_t start, uint64_t size);

    uint64_t GetStart();

    uint64_t GetSize();

    virtual ~Journal();

    TransactionPtr BeginTx();

    size_t GetBlockSize();

    Core::Error Unload();

private:
    Core::Error Replay();
    Core::Error StartCommitTx(Transaction* tx);
    Core::Error WriteTx(const TransactionPtr& tx, Core::NoIOBioList& bioList);
    void UnlinkTx(Transaction* tx, bool cancel);

    Core::Error ReadTxBlockComplete(Core::PageInterface& blockPage);
    Core::Error WriteTxBlockPrepare(Core::PageInterface& blockPage);

    JournalTxBlockPtr ReadTxBlock(uint64_t index, Core::Error& err);
    Core::Error WriteTxBlock(uint64_t index, const JournalTxBlockPtr& block, Core::NoIOBioList& bioList);

    Core::Error GetNextBlock(uint64_t& index);

    Core::Error Run(const Core::Threadable& thread) override;

    Core::Error Flush(Core::NoIOBioList& bioList);

private:

    Volume& VolumeRef;
    Core::HashTable<Guid, TransactionPtr, Core::RWSem, Core::Memory::PoolType::Kernel, 512> TxTable;
    Core::LinkedList<TransactionPtr, Core::Memory::PoolType::Kernel> TxList;
    Core::UniquePtr<Core::Thread> TxThread;
    Core::RWSem TxListLock;
    Core::Event TxListEvent;
    Core::RWSem Lock;
    Core::RingBuffer LogRb;
    Core::RWSem LogRbLock;
    uint64_t Start;
    uint64_t Size;
    unsigned int State;
};

}