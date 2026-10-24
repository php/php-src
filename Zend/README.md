# Zend Engine

## Zend memory manager

### General

The goal of the new memory manager (available since PHP 5.2) is to reduce memory
allocation overhead and speedup memory management.

### Debugging

Normal:

```bash
sapi/cli/php -r 'leak();'
```

Zend MM disabled:

```bash
USE_ZEND_ALLOC=0 valgrind --leak-check=full sapi/cli/php -r 'leak();'
```

### Request-scoped no-reuse mode

Set `ZEND_MM_NO_REUSE=1` in the process environment before starting PHP to
prevent native Zend MM from reusing freed allocation ranges within a request:

```bash
ZEND_MM_NO_REUSE=1 sapi/cli/php script.php
```

The default is disabled: leave the variable unset or set it to `0`. Only the
literal values `0` and `1` are accepted; other values cause an allocator panic.
This is not a `php.ini` option. It is read at engine startup, so changing it with
`putenv()` has no effect on the running engine. For web SAPIs, set it in the
launcher or service environment before PHP starts, and restart PHP processes
after changing it.

With the mode enabled:

- Freed small allocations, page runs, and huge mappings are cleared and retained
  until their heap shuts down, after request shutdown callbacks have finished.
  OS pages belonging to retired page runs and huge mappings are returned to the
  kernel on supported platforms while their virtual addresses remain reserved.
  Retired payloads are not used to store allocator free-list links.
- Reallocations of existing blocks always move to fresh storage, including
  shrinking and unchanged-size reallocations. The original storage is retired.
- Allocator garbage collection does not reclaim retired storage. Addresses may
  be reused after heap shutdown, normally in the next request.
- Retained mappings remain charged against `memory_limit`. Reaching the limit
  fails through the normal allocation-error path; it never enables reuse.
  `memory_get_usage(false)` can decrease after freeing an allocation even though
  the underlying storage remains retained.

Reserved virtual memory and allocator accounting approach cumulative allocation
volume rather than peak live memory. Resident memory may be lower where the
operating system can discard fully retired pages. Clearing freed storage and
moving reallocations also add memory bandwidth, system-call, and copying costs.
The mode is intended for selected short-lived requests, not long-running scripts
or application workers that do not perform normal PHP request shutdown.

The mode requires native Zend MM. Combining it with `USE_ZEND_ALLOC=0`, custom
heap or storage handlers, or `ZEND_MM_DEBUG` allocator wrappers causes an
allocator panic rather than silently bypassing the protection.

This is exploit mitigation, not memory safety or a sandbox. Invalid accesses
to retired storage may still occur without trapping. Persistent allocations and
extension-owned allocations outside native Zend MM are not covered, nor are
dangling references that survive heap shutdown.

See [Testing ZendMM No-Reuse Mode](../docs/source/miscellaneous/running-tests.rst#testing-zendmm-no-reuse-mode)
for regression-test instructions.

### Shared extensions

Since PHP 5.3.11 it is possible to prevent shared extensions from unloading so
that valgrind can correctly track the memory leaks in shared extensions. For
this there is the `ZEND_DONT_UNLOAD_MODULES` environment variable. If set, then
`DL_UNLOAD()` is skipped during the shutdown of shared extensions.

## ZEND_VM

`ZEND_VM` architecture allows specializing opcode handlers according to
`op_type` fields and using different execution methods (call threading, switch
threading and direct threading). As a result ZE2 got more than 20% speedup on
raw PHP code execution (with specialized executor and direct threading execution
method). As in most PHP applications raw execution speed isn't the limiting
factor but system calls and database calls are, your mileage with this patch
will vary.

Most parts of the old zend_execute.c go into `zend_vm_def.h`. Here you can find
opcode handlers and helpers. The typical opcode handler template looks like
this:

```c
ZEND_VM_HANDLER(<OPCODE-NUMBER>, <OPCODE>, <OP1_TYPES>, <OP2_TYPES>)
{
    <HANDLER'S CODE>
}
```

`<OPCODE-NUMBER>` is a opcode number (0, 1, ...)
`<OPCODE>` is an opcode name (ZEN_NOP, ZEND_ADD, :)
`<OP1_TYPES>` and `<OP2_TYPES>` are masks for allowed operand op_types.
Specializer will generate code only for defined combination of types. You can
use any combination of the following op_types UNUSED, CONST, VAR, TMP and CV
also you can use ANY mask to disable specialization according operand's op_type.
`<HANDLER'S CODE>` is a handler's code itself. For most handlers it stills the
same as in old `zend_execute.c`, but now it uses macros to access opcode
operands and some internal executor data.

You can see the conformity of new macros to old code in the following list:

```c
EXECUTE_DATA
    execute_data
ZEND_VM_DISPATCH_TO_HANDLER(<OP>)
    return <OP>_helper(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)
ZEND_VM_DISPATCH_TO_HELPER(<NAME>)
    return <NAME>(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)
ZEND_VM_DISPATCH_TO_HELPER_EX(<NAME>,<PARAM>,<VAL>)
    return <NAME>(<VAL>, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)
ZEND_VM_CONTINUE()
    return 0
ZEND_VM_NEXT_OPCODE()
    NEXT_OPCODE()
ZEND_VM_SET_OPCODE(<TARGET>
    SET_OPCODE(<TARGET>
ZEND_VM_INC_OPCODE()
    INC_OPCOD()
ZEND_VM_RETURN_FROM_EXECUTE_LOOP()
    RETURN_FROM_EXECUTE_LOOP()
ZEND_VM_C_LABEL(<LABEL>):
    <LABEL>:
ZEND_VM_C_GOTO(<LABEL>)
    goto <LABEL>
OP<X>_TYPE
    opline->op<X>.op_type
GET_OP<X>_ZVAL_PTR(<TYPE>)
    get_zval_ptr(&opline->op<X>, EX(Ts), &free_op<X>, <TYPE>)
GET_OP<X>_ZVAL_PTR_PTR(<TYPE>)
    get_zval_ptr_ptr(&opline->op<X>, EX(Ts), &free_op<X>, <TYPE>)
GET_OP<X>_OBJ_ZVAL_PTR(<TYPE>)
    get_obj_zval_ptr(&opline->op<X>, EX(Ts), &free_op<X>, <TYPE>)
GET_OP<X>_OBJ_ZVAL_PTR_PTR(<TYPE>)
    get_obj_zval_ptr_ptr(&opline->op<X>, EX(Ts), &free_op<X>, <TYPE>)
IS_OP<X>_TMP_FREE()
    IS_TMP_FREE(free_op<X>)
FREE_OP<X>()
    FREE_OP(free_op<X>)
FREE_OP<X>_IF_VAR()
    FREE_VAR(free_op<X>)
FREE_OP<X>_VAR_PTR()
    FREE_VAR_PTR(free_op<X>)
```

Executor's helpers can be defined without parameters or with one parameter. This
is done with the following constructs:

```c
ZEND_VM_HELPER(<HELPER-NAME>, <OP1_TYPES>, <OP2_TYPES>)
{
    <HELPER'S CODE>
}

ZEND_VM_HELPER_EX(<HELPER-NAME>, <OP1_TYPES>, <OP2_TYPES>, <PARAM_SPEC>)
{
    <HELPER'S CODE>
}
```

The executors code is generated by the PHP script `zend_vm_gen.php`. It uses
`zend_vm_def.h` and `zend_vm_execute.skl` as input and produces
`zend_vm_opcodes.h` and `zend_vm_execute.h`. The first file is a list of opcode
definitions. It is included from `zend_compile.h`. The second one is an executor
code itself. It is included from `zend_execute.c`.

`zend_vm_gen.php` can produce different kind of executors. You can select a
different opcode threading model using `--with-vm-kind=CALL|SWITCH|GOTO|HYBRID`.
You can disable opcode specialization using `--without-specializer`.
At last you can debug the executor using the original `zend_vm_def.h` or the
generated `zend_vm_execute.h` file. Debugging with the original file requires
the `--with-lines` option. By default, Zend Engine uses the following
command to generate the executor:

```bash
# Default VM kind is HYBRID
php zend_vm_gen.php --with-vm-kind=HYBRID
```
