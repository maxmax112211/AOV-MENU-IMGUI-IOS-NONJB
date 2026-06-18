#ifndef dobby_h
#define dobby_h

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  kMemoryOperationSuccess,
  kMemoryOperationError,
  kNotSupportAllocateExecutableMemory,
  kNotEnough,
  kNone
} MemoryOperationError;

typedef uintptr_t addr_t;
typedef uint32_t addr32_t;
typedef uint64_t addr64_t;

MemoryOperationError CodePatch(void *address, uint8_t *buffer, uint32_t buffer_size);

const char *DobbyBuildVersion();

int DobbyHook(void *address, void *replace_call, void **origin_call);

typedef struct {
  int hook_id;
  union {
    void *target_address;
    void *function_address;
    void *instruction_address;
  };
} HookEntryInfo;

typedef void (*DBICallTy)(void *ctx, const HookEntryInfo *info);

int DobbyInstrument(void *address, DBICallTy dbi_call);
int DobbyDestroy(void *address);

void *DobbySymbolResolver(const char *image_name, const char *symbol_name);

int DobbyGlobalOffsetTableReplace(char *image_name, char *symbol_name, void *fake_func, void **orig_func);

#if defined(__arm__) || defined(__arm64__) || defined(__aarch64__) || defined(__x86_64__)
void dobby_enable_near_branch_trampoline();
void dobby_disable_near_branch_trampoline();
#endif

typedef void (*linker_load_callback_t)(const char *image_name, void *handle);
void dobby_register_image_load_callback(linker_load_callback_t func);

#ifdef __cplusplus
}
#endif


#if defined(__arm64__) || defined(__aarch64__)

typedef union _FPReg {
  __int128_t q;
  struct { double d1, d2; } d;
  struct { float f1, f2, f3, f4; } f;
} FPReg;

typedef struct {
  uint64_t dmmpy_0;
  uint64_t sp;
  uint64_t dmmpy_1;

  union {
    uint64_t x[29];
    struct {
      uint64_t x0, x1, x2, x3, x4, x5, x6, x7, x8, x9,
               x10, x11, x12, x13, x14, x15, x16, x17,
               x18, x19, x20, x21, x22, x23, x24, x25,
               x26, x27, x28;
    } regs;
  } general;

  uint64_t fp;
  uint64_t lr;

  union {
    FPReg q[32];
    struct {
      FPReg q0, q1, q2, q3, q4, q5, q6, q7,
            q8, q9, q10, q11, q12, q13, q14, q15,
            q16, q17, q18, q19, q20, q21, q22, q23,
            q24, q25, q26, q27, q28, q29, q30, q31;
    } regs;
  } floating;

} RegisterContext;

#elif defined(__arm__)

typedef struct {
  uint32_t dummy_0;
  uint32_t dummy_1;
  uint32_t dummy_2;
  uint32_t sp;

  union {
    uint32_t r[13];
    struct {
      uint32_t r0, r1, r2, r3, r4, r5, r6, r7,
               r8, r9, r10, r11, r12;
    } regs;
  } general;

  uint32_t lr;
} RegisterContext;

#elif defined(__x86_64__)

typedef struct {
  uint64_t dummy_0;
  uint64_t rsp;

  union {
    struct {
      uint64_t rax, rbx, rcx, rdx, rbp, rsp,
               rdi, rsi, r8, r9, r10, r11,
               r12, r13, r14, r15;
    } regs;
  } general;

  uint64_t dummy_1;
  uint64_t flags;
} RegisterContext;

#endif


#define RT_FAILED -1
#define RT_SUCCESS 0

typedef enum {
  RS_FAILED = -1,
  RS_SUCCESS = 0
} RetStatus;


typedef struct {
  uint64_t hook_vaddr;
  uint64_t hook_size;
  uint64_t code_vaddr;
  uint64_t code_size;
  uint64_t patched_vaddr;
  uint64_t original_vaddr;
  uint64_t instrument_vaddr;
  uint64_t patch_size;
  uint64_t patch_hash;
  void *target_replace;
  void *instrument_handler;
} StaticInlineHookBlock;

int dobby_create_instrument_bridge(void *targetData);

bool dobby_static_inline_hook(
  StaticInlineHookBlock *hookBlock,
  StaticInlineHookBlock *hookBlockRVA,
  uint64_t funcRVA,
  void *funcData,
  uint64_t targetRVA,
  void *targetData,
  uint64_t InstrumentBridgeRVA,
  void *patchBytes,
  int patchSize
);

#endif
