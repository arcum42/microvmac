
#pragma once

#include "sys_dependencies.h"
#include "HW/KBRD/keycodes.h"
#include <array>

#define HaveMasterEvtQLock EmClassicKbrd
#if HaveMasterEvtQLock
extern uint16_t MasterEvtQLock;
#endif

typedef enum EvtQKind
{
    EvtQElKindKey,
    EvtQElKindMouseButton,
    EvtQElKindMousePos,
    EvtQElKindMouseDelta
} EvtQKind_t;

class EvtQEl
{
public:
    /* expected size : 8 bytes */
    EvtQKind_t kind;
    union
    {
        struct
        {
            uint8_t down;
            uint8_t key;
        } press;
        struct
        {
            uint16_t h;
            uint16_t v;
        } pos;
    } u;
};

/* my event queue */
const uint32_t EvtQLg2Sz = 4;
const uint32_t EvtQSize = (1 << EvtQLg2Sz);

class EventQueue
{
private:
    uint16_t In = 0;
    uint16_t Out = 0;
    const uint32_t Size = EvtQSize;
    const uint32_t IMask = (EvtQSize - 1);
    std::array<EvtQEl, EvtQSize> EvtQA;

public:
    bool NeedRecover = false;

    void OutDone(void);
    void TryRecoverFromFull(void);
    EvtQEl *PreviousIn(void);
    EvtQEl *OutP(void);
    EvtQEl *Alloc(void);
};

extern EventQueue EvtQ;

constexpr uint32_t kKeepMaskControl = (1 << 0);
constexpr uint32_t kKeepMaskCapsLock = (1 << 1);
constexpr uint32_t kKeepMaskCommand = (1 << 2);
constexpr uint32_t kKeepMaskOption = (1 << 3);
constexpr uint32_t kKeepMaskShift = (1 << 4);

void Keyboard_UpdateKeyMap(uint8_t key, bool down);
void MouseButtonSet(bool down);
void MousePositionSet(uint16_t h, uint16_t v);
void MousePositionNotify(int h, int v);
void InitKeyCodes(void);
void DisconnectKeyCodes(uint32_t KeepMask);