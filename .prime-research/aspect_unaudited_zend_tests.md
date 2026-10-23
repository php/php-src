# Exhaustive Audit: Unaudited Zend Tests & Core Language Mechanics

## 1. Overview & Scope
This audit covers the 49 Zend engine test suites omitted from earlier audit waves (all directories alphabetically >= `lazy_objects`):
`lazy_objects`, `line_numbers`, `list`, `lsb`, `magic_methods`, `match`, `multibyte`, `name_collision`, `named_params`, `namespaces`, `new_without_parentheses`, `nullable_types`, `nullsafe_operator`, `numeric_literal_separator`, `numeric_strings`, `object_types`, `objects`, `offsets`, `optimizer`, `parameter_default_values`, `partial_application`, `pipe_operator`, `prop_const_expr`, `property_hooks`, `readonly_classes`, `readonly_props`, `restrict_globals`, `return_types`, `serialize`, `shutdown_functions`, `stack_limit`, `static_variables`, `streams`, `switch`, `temporary_cleaning`, `this-reserved`, `throw`, `traits`, `try`, `type_coercion`, `type_declarations`, `unset`, `use_const`, `use_function`, `varSyntax`, `variadic`, `weakrefs`, `zend_ini`, `zpp`.

- **Total Test Files**: 2,626 `.phpt` regression test files
- **Total Explicit Bug/Regression Tests**: 503 files (marked with `bug*`, `gh*`, or crash/assertion signatures)
- **High-Risk Vulnerability Areas**: Lifecycle re-entrancy (property hooks, lazy objects, weakrefs, magic methods), VM live-range cleanup (temporary cleaning, exception unwinding), stack exhaustion limits, type invariant violations (typed properties, readonly mutation, reference coercion).

---

## 2. Quantitative Directory Breakdown

| Directory | Total Tests | Bug/GH Tests | Primary Vulnerability / Invariant Focus |
|---|---|---|---|
| `lazy_objects` | 230 | 44 | Realization re-entrancy, GC cycles during realize, proxy double-dispatch, bailing clone |
| `line_numbers` | 11 | 11 | AST node line mapping, error stack trace accuracy |
| `list` | 62 | 11 | Destructuring assignment, empty elements, reference unpacking |
| `lsb` | 25 | 3 | Late Static Binding scope resolution, inherited static calls |
| `magic_methods` | 170 | 112 | Destructor exception re-entrancy, __get/__set recursion guards, __toString leaks |
| `match` | 35 | 1 | Jump table optimization, strict comparison invariants, unhandled value exceptions |
| `multibyte` | 9 | 1 | Encoding conversion, multibyte identifier parsing |
| `name_collision` | 11 | 0 | Reserved keyword namespace resolution, class vs function collisions |
| `named_params` | 33 | 3 | Variadic unpacking, duplicate named parameters, positional-after-named ordering |
| `namespaces` | 114 | 7 | Symbol lookup shadowing, leading backslash resolution, aliasing |
| `new_without_parentheses` | 12 | 0 | Grammar ambiguities in member access on instantiate: `(new Foo)->bar()` |
| `nullable_types` | 11 | 0 | Nullable parameter/return coercions, variance constraints |
| `nullsafe_operator` | 48 | 4 | Short-circuiting, ref-assignment prevention, live range cleanup on null short-circuit |
| `numeric_literal_separator` | 11 | 2 | Lexer underscore handling in integers/floats, hex/bin literal boundaries |
| `numeric_strings` | 8 | 0 | String-to-number coercion deprecations, leading whitespace parsing |
| `object_types` | 11 | 0 | `object` typehint invariance, covariance/contravariance rules |
| `objects` | 33 | 0 | Object casting, hash collisions, comparison handlers |
| `offsets` | 20 | 1 | Array/string offset boundaries, undefined index notices |
| `optimizer` | 1 | 0 | SSA/DCE bytecode optimization sanity checks |
| `parameter_default_values` | 9 | 2 | Constant expressions as default parameter values, AST evaluation ordering |
| `partial_application` | 148 | 19 | Placeholder compilation, closure runtime cache double-free, attribute UAF |
| `pipe_operator` | 33 | 3 | LHS operand freeing on assert, AST compile null deref with `??=` |
| `prop_const_expr` | 20 | 0 | Property initializer constant expression AST evaluation |
| `property_hooks` | 217 | 46 | Object destruction during hook (UAF), cyclic leaks, asymmetric visibility, iterator crashes |
| `readonly_classes` | 25 | 8 | Trait mutable property injection (GH-9285), dynamic property disallowance |
| `readonly_props` | 47 | 1 | Initialization once-only invariant, reference assign prohibition, clone reinit rules |
| `restrict_globals` | 16 | 0 | De-registering / modifying `$GLOBALS` reference protections |
| `return_types` | 89 | 6 | Return type variance, covar/contra-variance, generator return types |
| `serialize` | 11 | 10 | Object deserialization gadgets, buffer overflows, refcount issues |
| `shutdown_functions` | 10 | 9 | Fatal error shutdown execution, internal trampoline crashes |
| `stack_limit` | 17 | 2 | Deep AST recursion, fiber stack limits, `__libc_stack_end` environment offset |
| `static_variables` | 13 | 0 | Local static variable binding, recursion persistence |
| `streams` | 10 | 10 | Custom stream wrapper callback re-entrancy and refcounting |
| `switch` | 12 | 9 | Jump table jump targets, type coercion matching, break unwinding |
| `temporary_cleaning` | 17 | 0 | VM live range register freeing, rope structure frees, exception cleanups |
| `this-reserved` | 11 | 3 | Prohibiting `$this` rebinding, unsetting, or parameter naming |
| `throw` | 5 | 1 | Throw expressions in ternary/coalescing operands |
| `traits` | 216 | 88 | Collision flattening, insteadof/as precedence, private shadow properties |
| `try` | 83 | 23 | Finally block unwind semantics, nested throw/catch cleanup |
| `type_coercion` | 73 | 3 | Strict vs weak scalar conversions, NaN conversion exception assertion |
| `type_declarations` | 512 | 14 | In-place assign_op type mutation UAF, reference coercion leaks |
| `unset` | 23 | 9 | Destroying variables during dtor, unsetting `$this`, float key unset |
| `use_const` | 11 | 0 | Group use import resolution for constants |
| `use_function` | 18 | 0 | Group use import resolution for functions |
| `varSyntax` | 20 | 0 | Uniform variable syntax, dereferencing arbitrary expressions |
| `variadic` | 21 | 2 | Variadic argument packing/unpacking, parameter passing by reference |
| `weakrefs` | 38 | 23 | WeakMap destruction order UAF, cyclic GC, reference assign during dtor |
| `zend_ini` | 14 | 3 | INI parsing edge cases, on_modify callbacks, per-dir overrides |
| `zpp` | 32 | 1 | Zend Parameter Parsing (ZPP) flags, type conversions, fast ZPP macros |

---

## 3. Recurring Vulnerability Mechanics & Crash Signatures

### A. Lifecycle Re-Entrancy & Object Destruction Hazards (UAF / Double Free)
1. **Property Hooks Re-Entrancy (GH-16040)**:
   - When a property hook (e.g., `get`) executes userland code that destroys the enclosing object (e.g., `$GLOBALS['a'] = null;`), the object instance is freed while the hook method is still active on the execution frame. Without internal refcount inc/dec guards on `$this`, reading or writing to the backing property leads directly to heap-use-after-free.
2. **WeakMap Destruction Order UAF (GH-18833, GH-17442, GH-13612)**:
   - WeakMap entries hold non-owning references to keys and owning references to values. If an object referenced in a WeakMap is destroyed during parameter passing (e.g. `headers_sent($obj, $generator)`), and its destructor throws or mutates the reference, internal structures are dereferenced after being freed. Cyclic structures between WeakMap keys and values require specialized GC collection algorithms (`gc_collect_cycles`) to prevent premature freeing.
3. **Lazy Object Realization Invariants (GH-15999, GH-20657, GH-18038)**:
   - Lazy proxies realize the underlying instance upon first property access. If the initializer closure destroys the proxy or drops its reference, realization code accesses freed memory. Furthermore, if realization triggers GC, resetting lazy state mid-flight corrupts proxy flags. Additionally, proxy forwarding repeatedly triggered magic methods (`__get`, `__set`) without setting proper recursion guards.

### B. VM Live Range Management & Operand Cleaning
1. **Live Range Leaks & Misclassification (`temporary_cleaning`)**:
   - The Zend VM uses `zend_live_range` to track variables that require destruction upon exception unwinding.
   - Tests uncover that non-zval temporary structures—such as `zend_rope` created during rope string concatenations—were incorrectly treated as `zval*` during cleanup unwinding, triggering invalid memory frees (`temporary_cleaning_015.phpt`).
   - Exceptions thrown during operand evaluation in nested ternary/switch or array destructuring left intermediate temporaries unfreed.

### C. Type System Invariants & Memory Mutation
1. **In-Place Mutation UAF on Typed Properties (`type_declarations/typed_properties_056`)**:
   - In `$o->foo += 5` where `$o->foo` is `string`, the binary op changes the zval to an `IS_LONG`. When the engine attempts to store the result back into `foo`, type validation fails with `TypeError`. If the VM does not roll back the original zval refcounts atomically, the original string or new temporary is freed twice or left dangling.
2. **Readonly Invariants & Clone Re-initialization (`readonly_props`, `readonly_classes`)**:
   - PHP enforces that readonly properties cannot be modified after initialization. The sole exception is `__clone()`, which permits re-initialization once. Tests enforce that:
     - Directly invoking `$obj->__clone()` is blocked from bypassing the readonly lock.
     - Traits cannot inject mutable properties into a readonly class (GH-9285).
     - Reference assignments to unset readonly properties (`$obj->prop =& $ref`) are prohibited (GH-7942).

### D. AST Compilation & Operator Pipeline Anomalies
1. **Pipe Operator AST compilation (`pipe_operator/gh22490`, `gh18965`)**:
   - When pipes are placed on the LHS of null-coalescing assignments (`[pipe expression]->p ??= 99`), the compiler AST traverser failed to handle the unassigned pipe node, resulting in null pointer dereference.
   - When assertions are disabled at compile-time (`zend.assertions=0`), `assert(...)` calls elided operand freeing, causing memory leaks of LHS piped inputs.
2. **Partial Function Application (PFA)**:
   - Closure runtime cache (`run_time_cache`) lifecycle was corrupted when partially applying `Closure::fromCallable()`, causing double-free on shutdown.
   - Attribute reflections on partially applied functions suffered from UAF when temporary attribute argument zvals were collected.

---

## 4. Key Takeaways & Defense-in-Depth Invariants
1. **Object Guards**: Any userland callback invocation (property hook, lazy initializer, destructor, magic method) must guarantee a temporary refcount increment on the containing instance to survive self-destruction.
2. **Live Range Ordering**: Exception unwind paths must strictly respect AST evaluation boundaries and distinguish primitive zvals from custom VM rope/closure buffers.
3. **Atomic Property Writebacks**: Compound assignments (`+=`, `.=`, `++`) on typed properties must validate types before modifying the storage location or modifying reference target pointers.
