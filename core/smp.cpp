#include "smp.h"
#include "atomic.h"

namespace Core
{

int Smp::GetCpuId()
{
    return get_kapi()->smp_processor_id();
}

void Smp::PreemptDisable()
{
    get_kapi()->preempt_disable();
}

void Smp::PreemptEnable()
{
    get_kapi()->preempt_enable();
}

void Smp::LockOnlineCpus()
{
    get_kapi()->get_online_cpus();
}

void Smp::UnlockOnlineCpus()
{
    get_kapi()->put_online_cpus();
}

void Smp::CallFunctionOtherCpus(void (*function)(void *data),
                                void *data, bool wait)
{
    get_kapi()->smp_call_function(function, data, wait);
}

void Smp::CountCpus(void *data)
{
    Atomic *counter = static_cast<Atomic *>(data);
    counter->Inc();
}

void Smp::Pause()
{
}

void Smp::BlockAndWait(void *data)
{
    struct BlockAndWait *bw = static_cast<struct BlockAndWait *>(data);

    bw->Counter->Inc();
    while (bw->Block->Get())
    {
        Pause();
    }
}

void Smp::CallFunctionCurrCpuOnly(void (*function)(void *data), void *data)
{
    Atomic counter, block;

    LockOnlineCpus();
    PreemptDisable();

    CallFunctionOtherCpus(&Smp::CountCpus, static_cast<void *>(&counter),
                          true);
    int nrCpus = counter.Get();

    counter.Set(0);
    block.Set(1);

    struct BlockAndWait bw;
    bw.Counter = &counter;
    bw.Block = &block;

    CallFunctionOtherCpus(&Smp::BlockAndWait,
                          static_cast<struct BlockAndWait *>(&bw), false);

    while (counter.Get() != nrCpus)
    {
        Pause();
    }

    function(data);

    block.Set(0);

    PreemptEnable();
    UnlockOnlineCpus();
}

}