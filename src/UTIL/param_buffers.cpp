
#include "UTIL/param_buffers.h"

extern uint8_t get_vm_byte(CPTR addr);
extern uint16_t get_vm_word(CPTR addr);
extern uint32_t get_vm_long(CPTR addr);

extern void put_vm_byte(CPTR addr, uint8_t b);
extern void put_vm_word(CPTR addr, uint16_t w);
extern void put_vm_long(CPTR addr, uint32_t l);

#if IncludePbufs
uint32_t PbufAllocatedMask;
uint32_t PbufSize[NumPbufs];
static void *PbufDat[NumPbufs];

bool PbufIsAllocated(tPbuf i)
{
    return ((PbufAllocatedMask & ((uint32_t)1 << (i))) != 0);
}

bool FirstFreePbuf(tPbuf *r)
{
    for (tPbuf i = 0; i < NumPbufs; ++i)
    {
        if (!PbufIsAllocated(i))
        {
            *r = i;
            return true;
        }
    }
    return false;
}

void PbufNewNotify(tPbuf Pbuf_No, uint32_t count)
{
    PbufSize[Pbuf_No] = count;
    PbufAllocatedMask |= ((uint32_t)1 << Pbuf_No);
}

void PbufDisposeNotify(tPbuf Pbuf_No)
{
    PbufAllocatedMask &= ~((uint32_t)1 << Pbuf_No);
}

MacErr_t CheckPbuf(tPbuf Pbuf_No)
{
    MacErr_t result;

    if (Pbuf_No >= NumPbufs)
    {
        result = mnvm_nsDrvErr;
    }
    else if (!PbufIsAllocated(Pbuf_No))
    {
        result = mnvm_offLinErr;
    }
    else
    {
        result = mnvm_noErr;
    }

    return result;
}

MacErr_t PbufGetSize(tPbuf Pbuf_No, uint32_t *Count)
{
    MacErr_t result = CheckPbuf(Pbuf_No);

    if (mnvm_noErr == result)
    {
        *Count = PbufSize[Pbuf_No];
    }

    return result;
}

MacErr_t PbufNewFromPtr(void *p, uint32_t count, tPbuf *r)
{
    tPbuf i;
    MacErr_t err;

    if (!FirstFreePbuf(&i))
    {
        free(p);
        err = mnvm_miscErr;
    }
    else
    {
        *r = i;
        PbufDat[i] = p;
        PbufNewNotify(i, count);
        err = mnvm_noErr;
    }

    return err;
}

void PbufKillToPtr(void **p, uint32_t *count, tPbuf r)
{
    *p = PbufDat[r];
    *count = PbufSize[r];

    PbufDisposeNotify(r);
}

MacErr_t PbufNew(uint32_t count, tPbuf *r)
{
    MacErr_t err = mnvm_miscErr;

    void *p = calloc(1, count);
    if (nullptr != p)
    {
        err = PbufNewFromPtr(p, count, r);
    }

    return err;
}

void PbufDispose(tPbuf i)
{
    void *p;
    uint32_t count;

    PbufKillToPtr(&p, &count, i);

    free(p);
}

void UnInitPbufs(void)
{
    for (tPbuf i = 0; i < NumPbufs; ++i)
    {
        if (PbufIsAllocated(i))
        {
            PbufDispose(i);
        }
    }
}

uint8_t *PbufLock(tPbuf i)
{
    return (uint8_t *)PbufDat[i];
}

void PbufTransfer(uint8_t *Buffer, tPbuf i, uint32_t offset, uint32_t count, bool IsWrite)
{
    void *p = ((uint8_t *)PbufDat[i]) + offset;
    if (IsWrite)
    {
        (void)memcpy(p, Buffer, count);
    }
    else
    {
        (void)memcpy(Buffer, p, count);
    }
}

static MacErr_t PbufTransferVM(CPTR Buffera,
                               tPbuf i, uint32_t offset, uint32_t count, bool IsWrite)
{
    MacErr_t result;
    uint32_t contig;
    uint8_t *Buffer;

label_1:
    if (0 == count)
    {
        result = mnvm_noErr;
    }
    else
    {
        Buffer = get_real_address0(count, !IsWrite, Buffera, &contig);
        if (0 == contig)
        {
            result = mnvm_miscErr;
        }
        else
        {
            PbufTransfer(Buffer, i, offset, contig, IsWrite);
            offset += contig;
            Buffera += contig;
            count -= contig;
            goto label_1;
        }
    }

    return result;
}

/* extension mechanism */

void ExtnParamBuffers_Access(CPTR p)
{
    MacErr_t result = mnvm_controlErr;

    switch (get_vm_word(p + ExtnDat_commnd))
    {
    case kCmndVersion:
        put_vm_word(p + ExtnDat_version, 1);
        result = mnvm_noErr;
        break;
    case kCmndPbufFeatures:
        put_vm_long(p + ExtnDat_params + 0, 0);
        result = mnvm_noErr;
        break;
    case kCmndPbufNew:
    {
        tPbuf Pbuf_No;
        uint32_t count = get_vm_long(p + ExtnDat_params + 4);
        /* reserved word at offset 2, should be zero */
        result = PbufNew(count, &Pbuf_No);
        put_vm_word(p + ExtnDat_params + 0, Pbuf_No);
    }
    break;
    case kCmndPbufDispose:
    {
        tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);
        /* reserved word at offset 2, should be zero */
        result = CheckPbuf(Pbuf_No);
        if (mnvm_noErr == result)
        {
            PbufDispose(Pbuf_No);
        }
    }
    break;
    case kCmndPbufGetSize:
    {
        uint32_t Count;
        tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);
        /* reserved word at offset 2, should be zero */

        result = PbufGetSize(Pbuf_No, &Count);
        if (mnvm_noErr == result)
        {
            put_vm_long(p + ExtnDat_params + 4, Count);
        }
    }
    break;
    case kCmndPbufTransfer:
    {
        uint32_t PbufCount;
        tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);
        /* reserved word at offset 2, should be zero */
        uint32_t offset = get_vm_long(p + ExtnDat_params + 4);
        uint32_t count = get_vm_long(p + ExtnDat_params + 8);
        CPTR Buffera = get_vm_long(p + ExtnDat_params + 12);
        bool IsWrite =
            (get_vm_word(p + ExtnDat_params + 16) != 0);
        result = PbufGetSize(Pbuf_No, &PbufCount);
        if (mnvm_noErr == result)
        {
            uint32_t endoff = offset + count;
            if ((endoff < offset) /* overflow */
                || (endoff > PbufCount))
            {
                result = mnvm_eofErr;
            }
            else
            {
                result = PbufTransferVM(Buffera,
                                        Pbuf_No, offset, count, IsWrite);
            }
        }
    }
    break;
    }

    put_vm_word(p + ExtnDat_result, result);
}
#endif