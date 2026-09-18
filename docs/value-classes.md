# Experimental value classes

This is a semantic research prototype on official `php/php-src` master,
PHP **8.6.0-dev**, based on commit
`13cec7c276033883e1e0ee22ea92fa1838b06dca` (2026-09-18).
The working branch is `experiment/value-class`. It uses ordinary Zend objects.
It does not implement a new value representation or promise a speedup.

## Build and run

The minimal build requires a C compiler, make, autoconf, bison and re2c:

```sh
./buildconf --force
./configure --disable-all --enable-debug --enable-tokenizer --disable-phpdbg --disable-cgi --without-pear
make -j12
sapi/cli/php -n your-script.php
```

For the measured release build, replace `--enable-debug` with `--disable-debug`.
Clean the build when changing build modes. OPcache and the JIT are included by
this branch's build configuration; they are disabled for the baseline timings.
Regenerate tokenizer data and Reflection metadata after editing their sources:

```sh
php ext/tokenizer/tokenizer_data_gen.php
php build/gen_stub.php ext/tokenizer/tokenizer_data.stub.php
php build/gen_stub.php ext/reflection/php_reflection.stub.php
```

In this workspace bison 3.8.2 and re2c 3.1 were extracted under
`/tmp/php-value-class-tools/root`, without a system installation. Builds used
that directory's `usr/bin` on `PATH` and its `usr/share/bison` as
`BISON_PKGDATADIR`. The system PHP generated the stub headers. The final usable
binary is `sapi/cli/php`; the debug binary was retained in
`/tmp/php-value-class-results/php-debug`.

## Supported syntax

```php
<?php
value class BookingId
{
    public function __construct(public string $value) {}

    public function toString(): string
    {
        return $this->value;
    }
}

$id = new BookingId('booking-123');
var_dump($id->value);                               // string(11) "booking-123"
var_dump((new ReflectionClass(BookingId::class))->isValue()); // true
```

```php
enum Currency { case EUR; }

value class Money
{
    public function __construct(
        public int $amount,
        public Currency $currency,
    ) {}
}

$money = new Money(100, Currency::EUR);
```

`value` is case insensitive and contextual: it is recognized only before a
class modifier sequence ending in `class`, separated by whitespace or comments.
Existing identifiers named `value`, including classes, types, functions,
methods, constants and namespaces, remain available. Explicit `final` and
`readonly` modifiers may appear once each in any order with `value`.
Repeated `value`, `abstract value`, and anonymous value classes are rejected.
The token `T_VALUE` is appended to the parser's token declarations, preserving
the existing token numbers.

## Exact semantics and limitations

| Area | Behavior |
| --- | --- |
| Class kind | A named class with a dedicated `ZEND_ACC_VALUE_CLASS` marker. |
| Finality | Implicitly final; extending it produces the existing final-class error. |
| Parent | A value class cannot extend any class, even a readonly or unresolved class. The compiler rejects the declaration before parent autoload. |
| Properties | Implicitly readonly, with the current engine's typed-property and initialization rules; promoted properties work. `mixed`, nullable and union types retain ordinary PHP behavior. |
| Mutation | An initialized property cannot be reassigned, incremented, unset, written through Reflection, or exposed as a writable reference. Array element writes through the property are rejected. |
| Defaults | Readonly property defaults are supported by this upstream branch and are preserved. |
| Initialization | A constructor is optional. It need not initialize every property. Uninitialized properties can still be initialized later by an allowed scope, including existing Reflection/serialization mechanisms. |
| Dynamic properties | Prohibited by the existing no-dynamic-properties flag. `#[AllowDynamicProperties]` is rejected by the readonly attribute validator. |
| Static properties and hooks | Rejected by existing readonly checks. Static methods and constants remain available. |
| Methods | Ordinary methods are allowed and need not be pure. Magic methods other than `__destruct()` and `__clone()` retain ordinary readonly behavior. |
| Destruction | A custom `__destruct()` is forbidden, including trait imports and aliases. Normal engine destruction still releases fields, which can themselves refer to objects with destructors. |
| Cloning | A custom `__clone()` is forbidden, including trait imports and aliases. `clone $object` is allowed and produces a distinct, shallow copy. `clone($object, [...])` keeps existing visibility/type/readonly rules and initializes the new copy without changing the original. |
| Interfaces | Supported, including `Stringable` and compatible property contracts. Existing interface restrictions still apply. |
| Traits | Existing readonly compatibility rules apply. Trait properties must already be readonly; value classes do not rewrite a trait's property declarations. Magic method restrictions are also enforced during trait binding. |
| Equality | `==` keeps ordinary PHP object comparison, including coercive field comparison. `===` keeps object identity. No interning, structural hashing or new array-key behavior. |
| Identity | `spl_object_id()`, `WeakReference`, `WeakMap`, aliases and object lifetime remain observable. |
| Serialization | Ordinary `O:` object format, class names, type checking and custom `__serialize()` / `__unserialize()` remain supported. Unserialization does not imply constructor execution. |
| Reflection | `ReflectionClass::isValue(): bool`, also inherited by `ReflectionObject`. `isFinal()` and `isReadOnly()` return true. Text output includes `value`; `getModifiers()` continues to expose final/readonly, without a new public `IS_VALUE` constant. |

**Readonly is shallow.** A contained object can mutate, and a reference already
inside a stored array can change its referent. Existing lazy-object Reflection
APIs are not restricted by this patch and retain their readonly-object behavior.
The flag is not proof of deep immutability, purity, complete initialization,
acyclicity, constructor validation, or unobservable identity. Optimizers must
prove the relevant additional properties at each use site.

Anonymous value classes, deep immutability, special collection layouts and
identity-free equality are outside this MVP. No serialization version tag or
cross-version transport guarantee is introduced. Third-party parsers will need
syntax support. Windows, 32-bit and ZTS builds have not been validated here.

## Phase 0 findings and integration

The scanner is `Zend/zend_language_scanner.l`, the grammar is
`Zend/zend_language_parser.y`, and class modifiers are combined through
`zend_add_class_modifier()`. Readonly class flags already drive both ordinary
and promoted property compilation, trait property checks, object-handler write
checks and attribute validation. Finality already prevents derived classes.

`zend_class_entry` in `Zend/zend.h` contains `uint32_t ce_flags` and an existing
`ce_flags2`. Inspection of all class flag definitions in `Zend/zend_compile.h`
confirmed that **bit 31 of `ce_flags` was unused for classes**. The prototype
uses `(1U << 31)`. That bit is used for a function flag in a different field;
the normal class/function flag namespaces already overlap. No structure was
enlarged and `ce_flags2` was not needed.

The AST carries only the explicit value marker. During class compilation the
compiler adds final, readonly and no-dynamic-properties flags before compiling
attributes and members. This lets redundant explicit final/readonly modifiers
work without falsely reporting duplicate implicit modifiers. The existing AST
printer also preserves `value`.

`zend_add_magic_method()` is shared by class compilation and trait method
binding. Its check rejects destructor/clone methods at that common point,
including aliases, without a separate trait implementation. The parent check
lives in `zend_compile_class_decl()`. No changes to inheritance, object
handlers, object allocation, GC, optimizer passes or JIT implementation are
needed for these semantics.

Reflection uses the existing class-flag helper, with a `uint32_t` mask to match
`ce_flags`. Stub files and generated metadata are updated together. Tokenizer
tables are regenerated through the repository generator, not edited by hand.
OPcache's existing class persistence carries the marker with the class entry.

There are two intentional changes to existing test expectations:

* Adding the fifth class-modifier alternative exceeds Bison's four-expected-token
  diagnostic limit. The three invalid `readonly enum/interface/trait` cases
  still fail, but their diagnostic no longer lists expected modifiers.
* Reflection's self-description gains `isValue()` and its method count changes
  from 64 to 65.

## Validation and measurements

See [the validation report](value-classes-validation.md),
[the complete changed-file list](value-classes-files.md), and
[the benchmark procedure and results](../benchmarks/value_classes/README.md).

## Optimization investigation

The following observations refer to the inspected checkout, not a proposed
alternative engine representation.

| Topic | Current implementation and consequence |
| --- | --- |
| Allocation | `ZEND_NEW` in `Zend/zend_vm_def.h` resolves/caches the class, calls `object_init_ex()`, and sets up a constructor call. `zend_objects_new()` in `Zend/zend_objects.c` uses `emalloc()`; standard initialization sets the refcount and registers an object-store handle. These costs remain. |
| Property layout | `zend_object::properties_table` in `Zend/zend_types.h` contains inline **zval slots** after the object header. `zend_object_properties_size()` in `Zend/zend_objects_API.h` sizes those slots and any guard. Types do not currently turn fields into unboxed scalar storage. A property HashTable can still be materialized for introspection even when dynamic properties are forbidden. |
| GC/refcounts | `zend_std_get_gc()` exposes the property table or materialized HashTable. Object references, object-store registration, field references, cycle checks and weak-reference notifications remain. Shallow readonly permits cycles through mutable objects or preexisting array references, so blanket `GC_NOT_COLLECTABLE` would be unsound. |
| Identity | Object handles, pointer identity, weak maps/references, escaping aliases and lifetime effects prevent unconditional merging or elimination. Destructor prohibition on the outer object does not remove destruction effects of its fields. |
| SSA/types | `Zend/Optimizer/zend_ssa.h` stores type masks, an exact/instanceof class entry, use/def chains, aliases and escape state. `zend_inference.c` recognizes `ZEND_NEW` as `MAY_BE_OBJECT` with refcount possibilities and exact class information when available; property reads use declared property types. There is no tuple of value-class fields. |
| Escape analysis | `Zend/Optimizer/escape_analysis.c` builds equi-escape sets and examines uses. Its allocation/local-definition checks for `ZEND_NEW` require, among other things, **no constructor**, no destructor, no parent, standard creation/constructor/destructor handlers and no `__get`/`__set`. Property reads can remain local; unknown calls, identities, object aliases and stores are conservative escape cases. A promoted constructor is currently excluded even if its body looks trivial. |
| Scalar replacement | `Zend/Optimizer/sccp.c` already has `PARTIAL_OBJECT`, a property-name map used only by analysis. `ZEND_NEW` seeds an **empty** map for a nonescaping allocation. `ct_eval_fetch_obj()` can read known entries. The assignment path explicitly declines potentially typed properties because conversion and errors would need modeling; it also requires the allow-dynamic-properties flag. A value-class flag alone does not make this path applicable. |
| Allocation elimination | `Zend/Optimizer/dce.c` already has a narrow rule for a nonescaping `NEW` immediately followed by `DO_FCALL`. Its safety depends on the current escape-analysis restrictions. Merely admitting constructors into that analysis could wrongly make constructor side effects removable. |
| Specialized opcodes | `ZEND_FETCH_OBJ_R` already caches class/offset information and can copy directly from an initialized slot. No new opcode is needed for the semantic POC or for constant property-read folding. |
| JIT | The current backend uses IR (`ext/opcache/jit/zend_jit_ir.c`). `zend_jit_fetch_obj()` specializes known properties and class guards; the function JIT's `ZEND_NEW` path in `zend_jit.c` still emits a VM handler call. Eliminating a virtual object in a trace would additionally need side-exit materialization and observer/exception handling. A trace 'escape' to the VM is not the same as SSA object escape analysis. |
| Arrays/collections | A packed PHP array still stores zvals referring to separately allocated objects. Neither the value flag nor shallow readonly flattens an array into records. A packed value collection would need a distinct layout and rules for mixed elements, aliases, references and extension APIs. |
| ABI/extensions | This patch adds a flag and API method without changing `zval`, `zend_object`, handlers or class-entry layout. It does not claim compatibility with prebuilt extensions from other PHP versions/builds. Any future unboxed representation would have to materialize normal objects at extension, Reflection, serialization, debugger, weak-reference and escaping-call boundaries. |

The runnable [optimizer probe](../benchmarks/value_classes/optimizer.php) was
dumped with `opcache.opt_debug_level=0x30000`. Both functions return 200, but
after optimization `doublePrice()` still contains `NEW`, argument sends,
`DO_FCALL`, `FETCH_OBJ_R` and arithmetic. Even the constructor-free
`doubleFixedPrice()` retains allocation and property access. Multiplication by
two becomes addition; neither object is replaced by a constant. The captured
dump is [optimizer-opcodes.txt](../benchmarks/value_classes/optimizer-opcodes.txt).

## One recommended next optimization

**Seed SCCP's existing partial-object map with initialized scalar defaults for
proven nonescaping, constructor-free value-class allocations.**

This is smaller than teaching the optimizer to evaluate arbitrary constructors
and is directly motivated by the `doubleFixedPrice()` dump. Limit the first
patch to known value classes passing the existing escape checks, ordinary
public readonly backed properties, and fully resolved `null`/boolean/integer/
float/string defaults. Exclude uninitialized slots, references, objects, arrays,
AST constants, property hooks, nonstandard handlers, dynamic names and
inaccessible properties. Preserve existing class-resolution and access errors.

The implementation belongs around SCCP's `ZEND_NEW` handling and its property
read evaluator: populate facts from `default_properties_table` using
`properties_info` for names, visibility and flags. Public read checks must be
explicit; the current partial-object map does not encode visibility. Retain the
existing conservative escape classification and DCE preconditions. Then reuse
`FETCH_OBJ_R` folding and arithmetic propagation. No new opcode, GC layout, or
JIT-only object representation is necessary.

The first acceptance case is `doubleFixedPrice()`: its property read and
arithmetic should fold to 200; any allocation removal must follow only from the
existing DCE proof. Negative tests must retain failures for inaccessible or
uninitialized fields and preserve all identity/escape/exception behavior. Test
both optimizer dumps and execution with OPcache off/on and JIT off/on, including
observer-enabled builds before shipping an optimization.

This intentionally does **not** optimize the promoted-constructor `Money`
example yet. Extending to that example requires a verified constructor summary
(initialization-only assignments, exact argument types or preserved conversions,
no publication of `$this`, calls or other effects), plus correct exception and
identity handling. The constructor-free step first exercises the existing
partial-object machinery without weakening those invariants. No optimization
pass is changed by this semantic patch.
