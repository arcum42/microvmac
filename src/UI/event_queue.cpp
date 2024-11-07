/* my event queue */
#include "event_queue.h"

EventQueue EvtQ;

static uint32_t theKeys[4];

static uint16_t MousePosCurV = 0, MousePosCurH = 0;
static bool MouseButtonState = false;

/* user event queue utilities */

#if HaveMasterEvtQLock
uint16_t MasterEvtQLock = 0;
/*
    Takes a few ticks to process button event because
    of debounce code of Mac. So have this mechanism
    to prevent processing further events meanwhile.
*/
#endif

EvtQEl *EventQueue::OutP(void)
{
    EvtQEl *p = nullptr;
    if (In != Out)
    {
        p = &EvtQA[Out & EvtQ.IMask];
    }
    return p;
}

void EventQueue::OutDone(void)
{
    ++Out;
}

/* events lost because of full queue */

EvtQEl *EventQueue::PreviousIn(void)
{
    EvtQEl *p = nullptr;
    if (In - Out != 0)
    {
        p = &EvtQA[(In - 1) & EvtQ.IMask];
    }

    return p;
}

EvtQEl *EventQueue::Alloc(void)
{
    EvtQEl *p = nullptr;
    if (In - Out >= Size)
    {
        NeedRecover = true;
    }
    else
    {
        p = &EvtQA[In & IMask];

        ++In;
    }

    return p;
}

void EventQueue::TryRecoverFromFull()
{
    MouseButtonSet(false);
    DisconnectKeyCodes(0);
}

void Keyboard_UpdateKeyMap(uint8_t key, bool down)
{
    const uint8_t k = key & 127; /* just for safety */
    const uint8_t bit = 1 << (k & 7);

    uint8_t *kp = (uint8_t *)theKeys;
    uint8_t *kpi = &kp[k / 8];
    bool CurDown = ((*kpi & bit) != 0);

    if (CurDown != down)
    {
        EvtQEl *p = EvtQ.Alloc();
        if (nullptr != p)
        {
            p->kind = EvtQElKindKey;
            p->u.press.key = k;
            p->u.press.down = down;

            if (down)
            {
                *kpi |= bit;
            }
            else
            {
                *kpi &= ~bit;
            }
        }

        QuietEnds();
    }
}

void MouseButtonSet(bool down)
{
    if (MouseButtonState != down)
    {
        EvtQEl *p = EvtQ.Alloc();
        if (nullptr != p)
        {
            p->kind = EvtQElKindMouseButton;
            p->u.press.down = down;

            MouseButtonState = down;
        }

        QuietEnds();
    }
}

void MousePositionSet(uint16_t h, uint16_t v)
{
    if ((h != MousePosCurH) || (v != MousePosCurV))
    {
        EvtQEl *p = EvtQ.PreviousIn();
        if ((nullptr == p) || (EvtQElKindMousePos != p->kind))
        {
            p = EvtQ.Alloc();
        }
        if (nullptr != p)
        {
            p->kind = EvtQElKindMousePos;
            p->u.pos.h = h;
            p->u.pos.v = v;

            MousePosCurH = h;
            MousePosCurV = v;
        }

        QuietEnds();
    }
}

void InitKeyCodes(void)
{
    theKeys[0] = theKeys[1] = theKeys[2] = theKeys[3] = 0;
}

void DisconnectKeyCodes(uint32_t KeepMask)
{
    /*
        Called when may miss key ups,
        so act is if all pressed keys have been released,
        except maybe for control, caps lock, command,
        option and shift.
    */

    for (int j = 0; j < 16; ++j)
    {
        uint8_t k1 = ((uint8_t *)theKeys)[j];
        if (0 != k1)
        {
            uint8_t bit = 1;
            for (int b = 0; b < 8; ++b)
            {
                if (0 != (k1 & bit))
                {
                    int key = j * 8 + b;
                    uint32_t m;

                    switch (key)
                    {
                    case MKC_Control:
                        m = kKeepMaskControl;
                        break;
                    case MKC_CapsLock:
                        m = kKeepMaskCapsLock;
                        break;
                    case MKC_Command:
                        m = kKeepMaskCommand;
                        break;
                    case MKC_Option:
                        m = kKeepMaskOption;
                        break;
                    case MKC_Shift:
                        m = kKeepMaskShift;
                        break;
                    default:
                        m = 0;
                        break;
                    }
                    if (0 == (KeepMask & m))
                    {
                        Keyboard_UpdateKeyMap(key, false);
                    }
                }
                bit <<= 1;
            }
        }
    }
}

bool FindKeyEvent(int *VirtualKey, bool *KeyDown)
{
    EvtQEl *p;

    if (
#if HaveMasterEvtQLock
        (0 == MasterEvtQLock) &&
#endif
        (nullptr != (p = EvtQ.OutP())))
    {
        if (EvtQElKindKey == p->kind)
        {
            *VirtualKey = p->u.press.key;
            *KeyDown = p->u.press.down;
            EvtQ.OutDone();
            return true;
        }
    }

    return false;
}