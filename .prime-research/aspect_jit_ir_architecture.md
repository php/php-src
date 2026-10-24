# Deep Architectural Analysis: PHP JIT IR Compiler & Intermediate Representation Pipeline

## 1. Executive Overview
PHP's JIT compiler (introduced in PHP 8.0 and modernized with the standalone lightweight IR framework authored by Dmitry Stogov) translates Zend Virtual Machine (VM) bytecode (opcodes) into native machine code. It supports function-level JIT and tracing JIT (`opcache.jit=tracing`). The JIT architecture is bifurcated into:
1. **Front-End / Integration Layer (`ext/opcache/jit/zend_jit_ir.c`, `zend_jit_trace.c`, `zend_jit.c`)**: Lowers Zend bytecode, SSA data-flow, and type feedback into IR nodes; manages type guards, deoptimization snapshots, inline caches, and VM frame syncing.
2. **Back-End IR Engine (`ext/opcache/jit/ir/`)**: A self-contained, SSA-based optimizing compiler framework providing constant folding, SCCP, global code motion, linear scan register allocation, and target code generation via DynASM for x86_64, i386, and AArch64.

---

## 2. JIT IR Compilation Phases & Pipeline Orchestration
The compilation pipeline for a trace or function is driven by `zend_jit_ir_compile()` (`zend_jit_ir.c:2839`):

```
       Zend Bytecode / Tracing
                  │
                  ▼
   1. Front-End IR Construction (ir_START, ir_fold)
                  │
                  ▼
   2. Def-Use Lists Generation (ir_build_def_use_lists)
                  │
                  ▼
   3. Sparse Conditional Constant Propagation (ir_sccp)
                  │
                  ▼
   4. Control Flow Graph & Dominators (ir_build_cfg, ir_build_dominators_tree, ir_find_loops)
                  │
                  ▼
   5. Global Code Motion (ir_gcm - Cliff Click PLDI'95)
                  │
                  ▼
   6. Local Scheduling (ir_schedule)
                  │
                  ▼
   7. Target Instruction Matching (ir_match - BURS/Dynamic Programming)
                  │
                  ▼
   8. Virtual Register Assignment (ir_assign_virtual_registers)
                  │
                  ▼
   9. Liveness & Interval Building (ir_compute_live_ranges)
                  │
                  ▼
  10. Interval Coalescing (ir_coalesce)
                  │
                  ▼
  11. Linear Scan Register Allocation (ir_reg_alloc - Wimmer & Franz CGO'10)
                  │
                  ▼
  12. Block Layout Scheduling (ir_schedule_blocks)
                  │
                  ▼
  13. Machine Code Emission (ir_emit_code via DynASM)
```

### Detailed Phase Mechanics:
1. **IR Construction & On-the-Fly Folding**: Instructions grow forward (indices `> 0`), while deduplicated constants grow backward (indices `< 0`). Expressions are evaluated through `ir_fold()` which attempts immediate reduction.
2. **Def-Use Lists (`ir_build_def_use_lists`)**: Populates `ctx->use_lists` and `ctx->use_edges` arrays, enabling O(1) traversal from definitions to all user instructions.
3. **SCCP (`ir_sccp.c`)**: Implements Wegman & Zadeck's lattice algorithm. Eliminates dead branches and simplifies control-flow merges based on constants and combined copy propagation.
4. **CFG & Dominance Tree (`ir_cfg.c`)**: Constructs basic blocks (`ir_block`), discovers natural loops, and computes immediate dominators (`idom`) and dominance frontiers using Lengauer-Tarjan or iterative semi-dominators.
5. **GCM (`ir_gcm.c`)**: Places floating data-flow instructions into blocks:
   - `schedule_early`: Pushes nodes to the earliest block dominating all inputs.
   - `schedule_late`: Pushes nodes to the latest common ancestor (LCA) in the dominator tree across all uses.
   - Selection chooses the block along the projection with the minimum loop depth, hoisting loop-invariant computations automatically.
6. **Local Scheduling (`ir_schedule`)**: Topologically sorts data dependencies within each basic block, minimizing live register pressure.
7. **Target Matching (`ir_match`)**: Matches IR trees into target-specific machine instructions and addressing modes (`[base + index*scale + disp]`) defined in `ir_x86.dasc` or `ir_aarch64.dasc`.
8. **SSA Linear Scan Register Allocation (`ir_ra.c`)**: Allocates physical registers or stack spill slots according to Wimmer & Franz's algorithm.
9. **Code Emission (`ir_emit_code`)**: Emits binary code into DynASM buffers, resolves relocations, applies branch veneers (AArch64), and handles snapshot registration callbacks.

---

## 3. Register Allocation Engine (`ir_ra.c`)
The IR allocator implements **Linear Scan on SSA Form** (Christian Wimmer & Michael Franz, CGO 2010) with optimized interval splitting (VEE 2005):
- **Virtual Register Assignment**: Non-fused data and memory nodes with multiple uses receive unique virtual register IDs (`ctx->vregs`).
- **Live Interval Construction**: Lifetime positions are numbered sequentially (step of 2). Loops extend intervals to the loop header and back edges to prevent premature eviction within loops.
- **Interval Queues**: Intervals move through `unhandled`, `active`, `inactive`, and `handled` states:
  - `ir_try_allocate_free_reg()`: Attempts to find an unblocked physical register without splitting.
  - `ir_allocate_blocked_reg()`: If no free register is found, the allocator selects a victim with the furthest next use or lowest spill weight, splitting the interval at optimal block boundaries or before the next use position.
- **SSA Deconstruction & Parallel Moves**:
  - Moves required by $\Phi$ functions and split ranges are computed in `ir_compute_dessa_moves()`.
  - Parallel copies are sorted to avoid clobbering; cycles are detected and broken via `ir_dessa_resolve_cycle()` using dedicated temporary registers (`tmp_reg` or FP scratch).
- **Spill Management**: Spills use designated CPU stack frames (`fixed_stack_frame_size`) or special VM stack frames (`spill_base = ZREG_FP`).

---

## 4. Constant Folding & Propagation Engine (`ir_fold.h`, `ir_sccp.c`)
- **Folding Engine (`ir_fold.h`, `ir.c`)**:
  - Heavily inspired by Mike Pall's LuaJIT folding engine.
  - Operates using a semi-perfect hash table generated at build time (`gen_ir_fold_hash.c`) keyed by `(opcode | (op1.type << 7) | (op2.type << 14))`.
  - Probes exact patterns first, then wildcard masks (`(op2, op1, op) -> (op2, _, op) -> (_, op1, op) -> (_, _, op)`).
  - Handles algebraic identities ($x + 0 = x$, $x \times 1 = x$, $x \oplus x = 0$), bitwise shifts, constant reassociation, relational inversions, and overflows (`ADD_OV`, `SUB_OV`).
  - Falls back to Local Common Subexpression Elimination (CSE) via backward hash chaining (`_ir_fold_cse`).
- **Sparse Conditional Constant Propagation (`ir_sccp.c`)**:
  - 3-level lattice: $\top$ (unvisited/unreached), Constant/Copy, $\bot$ (overdefined).
  - Propagates values across both executable CFG edges and SSA data-flow edges simultaneously.
  - Evaluates conditional branches (`IR_IF`, `IR_SWITCH`); if the condition is constant, unfeasible successor branches are pruned, which cascades into pruning dead $\Phi$ inputs and unreachable blocks.

---

## 5. Type Guards & Speculative Optimization
- Traced execution records observed runtime types in `zend_jit_trace_rec`.
- During compilation, speculative type assertions are inserted:
  - `jit_guard_Z_TYPE()` emits `ir_GUARD(ir_EQ(type_ref, expected_type), exit_addr)`.
  - Negative guards use `ir_GUARD_NOT()`.
  - Guards on class instances verify `object->ce == expected_ce`.
  - Arithmetic operations guard against integer overflow (`ir_OVERFLOW`) or division by zero.
- Guard failures immediately branch to dedicated exit stubs that initiate deoptimization.

---

## 6. Deoptimization State Machine & Stack Frame Reconstruction
Deoptimization bridges native optimized code back to the interpreter without losing or corrupting state:
1. **Snapshot Creation (`jit_SNAPSHOT`, `zend_jit_ir.c:614`)**:
   - For every guard and side exit, a snapshot records all live variables across the PHP VM frame (`op_array->last_var + op_array->T`).
   - Stores SSA references to all values currently held in registers or spill slots.
2. **Snapshot Resolution Handler (`zend_jit_snapshot_handler`, `zend_jit_ir.c:756`)**:
   - Triggered post-RA during machine code emission via `IR_SNAPSHOT_HANDLER`.
   - Maps each variable in the snapshot to its allocated physical location:
     - `ZREG_NONE`: Undefined or unmodified in memory.
     - `IR_REG_NUM(reg)`: Active CPU general-purpose or floating-point register.
     - `IR_REG_SPILL_LOAD`: Spilled to CPU stack slot (recorded with relative offset).
     - `IR_REG_SPILL_SPECIAL`: Spilled to PHP VM frame.
     - `ZREG_CONST`: Immediate folded constant value.
   - If register mappings conflict across shared exit points, `zend_jit_duplicate_exit_point()` forks a new exit stub.
3. **Runtime Exit & Reconstruction (`zend_jit_trace_exit`, `zend_jit_trace.c:8710`)**:
   - Native guard jump executes `ir_EXITCALL(zend_jit_trace_exit)`, saving all CPU GP and FP registers into `zend_jit_registers_buf`.
   - Iterates over `t->stack_map` for the exit point:
     - Unboxes raw integers and doubles from registers or stack spill slots back into `zval` format on the VM frame (`ZVAL_LONG`, `ZVAL_DOUBLE`).
     - Restores `zval.u1.type_info` for type-only tracked variables.
     - Reconstructs object references (`ZREG_THIS`), handles refcounts (`ZREG_ZVAL_ADDREF`), and cleans up partially evaluated operands (`ZEND_JIT_EXIT_FREE_OP1/OP2`).
     - Restores pending nested call structures (`EX(call)` via `ZEND_JIT_EXIT_RESTORE_CALL`).
     - Sets VM program counter `EX(opline)`.
4. **Resumption / Side Tracing**:
   - If the side exit counter exceeds hotness thresholds (`zend_jit_trace_exit_is_hot()`), a new side trace is spawned directly from the exit point.
   - Otherwise, returns to interpreter dispatch via `zend_jit_trace_exit_to_vm()`.

---

## 7. Polymorphic Inline Caches & VM/Native Boundaries
- **Monomorphic Inline Cache Fast-Path**:
  - Compares the object class pointer `obj->ce` against the cached `ce` in `run_time_cache[opline->result.num]`.
  - On match, directly loads cached `zend_function*` from `run_time_cache + sizeof(void*)`.
  - On miss, takes cold path to `zend_jit_find_method_helper()` to perform dynamic class hierarchy lookup and update the cache.
- **Polymorphic Method Call Deoptimization**:
  - When a call site is polymorphic, the trace records `ZEND_JIT_EXIT_METHOD_CALL`.
  - Snapshots capture two extra references: `poly_func` and `poly_this`.
  - When the exit becomes hot, a polymorphic side trace is compiled with `polymorphic_side_trace = true`.
  - The side trace loads `func_ref` and `this_ref` directly from parent snapshot registers or spill slots (`zend_jit_deopt_rload_spilled`), avoiding redundant method resolution.
- **Native / VM Boundary Contracts**:
  - Pinned global registers (via GCC global register variables):
    - x86_64: `execute_data` = `%r14`, `opline` = `%r15`.
    - i386: `execute_data` = `%esi`, `opline` = `%edi`.
    - AArch64: `execute_data` = `x27`, `opline` = `x28`.
  - Spill Base: `ctx->spill_base` is anchored to `execute_data` (`ZREG_FP`), permitting efficient zero-cost addressing into PHP VM variables.
  - Native function calls conform strictly to System V / Windows x64 ABIs with automated shadow space and caller-saved/callee-saved register management.

---

## 8. Summary Architecture Metrics
| Metric | Value / Detail |
|---|---|
| IR Opcodes Count | 139 core opcodes (`IR_OPS` in `ir.h`) |
| IR Types | 13 basic types (`BOOL`, `U8..U64`, `I8..I64`, `ADDR`, `CHAR`, `FLOAT`, `DOUBLE`) + SIMD vector types |
| Supported Target ISAs | x86_64, i386, AArch64 |
| Target Register Pools | x86_64: 16 GP + 16 XMM; AArch64: 31 GP + 32 FP/SIMD |
| Pinned Registers | Frame pointer `execute_data` (`r14`/`x27`), Opline IP (`r15`/`x28`), Stack pointer |
| RA Theoretical Basis | Wimmer & Franz (CGO'10) SSA Linear Scan with Split Intervals |
| GCM Theoretical Basis | Cliff Click (PLDI'95) Early/Late Dominance Placement |
| SCCP Theoretical Basis | Wegman & Zadeck (TOPLAS'91) Sparse Conditional Constant Propagation |
