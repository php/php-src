###############
 Generic types
###############

PHP supports generic type parameters on classes, interfaces, traits, functions, methods, closures,
and arrow functions. They are *bound-erased*: at runtime, each type parameter is replaced by its
declared bound (or ``mixed`` when unbounded), so the engine sees ordinary PHP types. The
pre-erasure form is kept in a per-entity side table and consulted by the linker, the runtime
turbofish check, and Reflection. Existing handlers - parameter checks, return-type verification,
property writes - operate on the erased type and never see a ``T``-ref.

***************************
 Pre-erasure type carriers
***************************

``zend_type`` carries the pre-erasure shapes generics need. Two bits in ``type_mask`` distinguish
them:

.. code:: c

   #define _ZEND_TYPE_TYPE_PARAMETER_BIT  (1u << 25)  /* t.ptr is a zend_type_parameter_ref * */
   #define _ZEND_TYPE_NAMED_WITH_ARGS_BIT (1u << 31)  /* t.ptr is a zend_type_named_with_args * */

When ``_ZEND_TYPE_TYPE_PARAMETER_BIT`` is set, ``t.ptr`` is a reference to a declared parameter;
when ``_ZEND_TYPE_NAMED_WITH_ARGS_BIT`` is set, ``t.ptr`` is a generic application like
``Foo<int, T>``:

.. code:: c

   typedef struct _zend_type_parameter_ref {
       zend_string *name;     /* "T" */
       uint32_t     index;    /* position in the parameter list */
       uint8_t      origin;   /* CLASS_LIKE or FUNCTION_LIKE */
   } zend_type_parameter_ref;

   typedef struct _zend_type_named_with_args {
       zend_string *name;     /* "Foo", or NULL for synthetic turbofish carriers */
       uint32_t     name_attr;
       uint32_t     count;
       zend_type    args[1];  /* flexible array */
   } zend_type_named_with_args;

The ``origin`` field on a ``T``-ref tells you which scope to resolve ``index`` in. NAMED_WITH_ARGS
values with ``name == NULL`` are synthetic carriers that hold the args at a turbofish site -
they're not type expressions, just a payload. Both payloads only appear in side-table slots, are
heap-allocated, and are released through ``zend_type_release``.

**********************
 Per-entity metadata
**********************

Both ``zend_class_entry`` and ``zend_op_array`` carry two pointer fields for generic information,
both ``NULL`` on entities that don't use generics:

.. code:: c

   zend_generic_parameter_list *generic_parameters;  /* the entity's own <T, U> declaration */
   zend_generic_type_table     *generic_types;       /* pre-erasure metadata for everything else */

``generic_parameters`` is the entity's own declaration list (a flexible-array struct allocated
through ``zend_generic_parameter_list_alloc``). Each parameter:

.. code:: c

   typedef struct _zend_generic_parameter {
       zend_string         *name;
       zend_generic_variance variance;          /* INVARIANT, COVARIANT, CONTRAVARIANT */
       zend_type            bound;              /* erased view; NONE if unbounded */
       zend_type            bound_pre_erasure;  /* NONE if same as bound */
       zend_type            default_type;
       zend_type            default_pre_erasure;
   } zend_generic_parameter;

``generic_types`` collects every other place a pre-erasure type might need to be looked up:

.. code:: c

   typedef struct _zend_generic_type_table {
       zend_type   *return_type;
       zend_type   *extends;
       HashTable   *parameters;        /* parameter index -> zend_type * */
       HashTable   *properties;        /* zend_string *  -> zend_type * */
       HashTable   *class_constants;
       HashTable   *implements;        /* implements index -> zend_type * */
       HashTable   *trait_uses;        /* trait-use index -> zend_type * */
       HashTable   *turbofish_args;    /* opline->extended_value -> zend_type * */
   } zend_generic_type_table;

Every slot is independently NULLable. The table is allocated lazily by
``zend_generic_get_or_create_class_table`` / ``..._op_array_table`` on first use, and individual
slots stay ``NULL`` whenever the pre-erasure form would be byte-equal to the erased one. You
populate slots through the ``zend_generic_type_table_set_*`` family; each setter takes ownership
of the ``zend_type`` you hand it.

The ``turbofish_args`` table is keyed by ``opline->extended_value`` rather than the opline's
position in the bytecode array. Optimizer passes reorder, insert, and delete opcodes; the
``extended_value`` operand belongs to the opline itself, so it survives reordering.

Attributes carry their turbofish on two fields on ``zend_attribute``:

.. code:: c

   uint8_t      generic_arity;
   zend_type   *generic_args;       /* NAMED_WITH_ARGS holding turbofish args */

******************
 Compile-time flow
******************

When the compiler reaches a generic declaration, ``zend_compile_generic_type_parameter_list``
builds the parameter list and pushes a linked-list scope entry. ``T``-ref resolution walks the
chain through ``zend_generic_lookup``. The scope entry's ``self_compiling`` field points at the
parameter whose bound or default is currently being compiled, which is what lets the compiler
reject ``class A<T : T>`` (top-level self-reference) while accepting
``class A<T : Comparable<T>>`` (the inner ``T`` is nested under another generic).

Every type expression carrying a ``T`` is compiled twice: once as the runtime-erased ``zend_type``
on the entity's ordinary slot (``arg_info::type``, ``zend_property_info::type``), and once in
pre-erasure form on the matching ``generic_types`` slot. When the bound is a list type (union,
intersection, or DNF), the erased copy is built through ``zend_arena_deep_copy_type_list`` so
nested type lists are not aliased between ``param->bound`` and the consuming ``arg_info``.

For a turbofish at a call or ``new`` site, ``zend_emit_verify_generic_arguments`` compiles the
args into a NAMED_WITH_ARGS payload, stores it in ``op_array->generic_types->turbofish_args``
under a fresh ``args_id``, and emits a ``ZEND_VERIFY_GENERIC_ARGUMENTS`` opcode with
``op2.num = arity`` and ``extended_value = args_id``. The opcode sits between the call's
``INIT_*`` (or ``ZEND_NEW``) and its ``DO_*``, so the call frame is set up when the handler runs.

******************************
 Variance and static context
******************************

After a class or function with non-invariant generic parameters compiles,
``zend_check_generic_variance_markers`` and ``zend_check_function_variance_markers`` walk every
position where a ``T`` can appear (signatures, properties, hook signatures, the type-args at
``extends``/``implements``/``use``, bounds, defaults). Both go through ``zend_variance_walk``,
which dispatches each ``T``-ref to the matching parameter list by ``ref->origin``.

Polarity composes through nested generics: outer × slot, with INVARIANT absorbing. Method return
types are covariant, parameters contravariant; readonly and get-only-hooked properties covariant,
set-only-hooked contravariant, r/w and get+set hooked invariant; bounds and defaults always
invariant. Constructors are exempt. Violations produce ``Type parameter T declared covariant
(+T) cannot appear in contravariant position``.

The static-context check is structural: ``zend_check_class_origin_in_static_context`` rejects any
class-origin ``T``-ref that resolves while ``CG(in_static_member_type)`` is set. The flag is set
in ``zend_compile_func_decl`` around the signature of a static method and in
``zend_compile_prop_decl`` around the type of a static property - a static member has no instance,
so an instance-bound parameter has no binding to consult.

*****************
 Runtime handler
*****************

The ``ZEND_VERIFY_GENERIC_ARGUMENTS`` handler lives in ``zend_vm_def.h``. It looks up the captured
args via ``zend_generic_get_turbofish_args`` and dispatches to ``zend_check_generic_call_arguments``
(call) or ``zend_check_generic_new_arguments`` (instantiation). Both validate arity against the
callee's ``generic_parameters``, then walk the carrier and call
``zend_check_generic_arg_satisfies_bound`` for each ``(arg, parameter)`` pair. Mismatches throw
``ArgumentCountError`` or ``TypeError``; on exception, teardown releases the call frame, the
trampoline name if any, and ``$this`` if held.

************************
 Inheritance and linking
************************

When a child extends a generic ancestor or uses a generic trait, the linker substitutes the
inherited prototype's pre-erasure types with the child's bindings before the variance check runs.
``zend_get_inheritance_binding`` returns the *direct* binding (the args at ce's own
``extends``/``implements``/``use`` clause); for ``extends`` it matches by pointer when
``ZEND_ACC_RESOLVED_PARENT`` is set and by name otherwise, since the override check fires before
parent resolution on the deferred-obligation path. ``zend_get_inheritance_binding_full`` walks
the parent chain and direct interfaces, composing each link through
``zend_substitute_leaf_type_param`` and looking the parent up by name if it isn't yet resolved.

``zend_substitute_leaf_type_param`` replaces a class-scope ``T``-ref with ``args[T->index]`` and
propagates the nullable bit, so ``?T`` with ``T = int`` substitutes to ``?int``. Other modifier
bits are not propagated; non-leaf carriers (NAMED_WITH_ARGS, lists) are returned unchanged.
``zend_substitute_proto_type`` is the entry point used by ``zend_do_perform_implementation_check``;
it returns the unsubstituted fallback when ce isn't generic, when the binding lookup fails, or
when substitution would yield another ``T``-ref because the inheriting class forwards the
parameter. ``zend_get_function_declaration`` takes an optional ``subst_ce``: when present, each
printed type runs through ``zend_substitute_proto_type`` so the LSP error message shows the
substituted parent signature rather than the bound-erased form.

***************************
 Member substitution sites
***************************

Substitution is applied at five sites. Each clones the parent's ``zend_property_info`` or
``zend_function``, allocates a fresh ``arg_info`` in the child's arena, and shares body opcodes
with the parent via refcount. Clones keep ``ce`` set to the parent's defining class so
``zend_opcode.c``'s teardown does not double-release shared fields.

-  ``do_inherit_property`` - property type on a class extending a generic parent.
-  Property hook signatures - the ``get`` return slot and the ``set`` value-parameter slot.
-  ``zend_do_traits_property_binding`` - trait property type, with the using class's binding.
-  ``zend_add_trait_method`` - trait method signature, via ``zend_substitute_trait_method_arg_info``.
-  ``do_inherit_method`` - non-overridden inherited method signature, via
   ``zend_maybe_substitute_inherited_method``.

Body opcodes are not re-emitted: ``VERIFY_*`` opcodes were laid down at the parent's compile time
against the unsubstituted view, so the child observes the substituted signature on the clone but
the parent's original opcodes inside.

Diamond detection is in ``zend_validate_generic_diamond_bindings``. It runs *before* ``ce->parent`` is set and
``ZEND_ACC_RESOLVED_PARENT`` is established; the side-table accessors bypass the resolved-parent
gate, so detection sees the bindings directly. For each direct binding source of ce (parent plus
each ``implements`` entry), the check composes the ce-to-source binding with
``zend_get_inheritance_binding_full`` for every generic target reachable from source. Records are
stored in a transient ``HashTable`` keyed by target ``zend_class_entry *``;
``zend_diamond_record_or_check`` compares only arity. Differing arity is structurally
inconsistent and fires ``zend_error_noreturn`` with both source paths. Differing args at matching
arity are admitted at this stage and resolved downstream - the interface-level merge synthesises
a use-site-variance-aware contract on the inheriting interface, and per-path LSP verifies any
concrete implementer against each substituted parent prototype.

``zend_check_generic_link_bounds`` validates each supplied arg against the corresponding target
parameter's bound. When the supplied arg is a leaf class-scope ``T``-ref of ce, the effective arg
type is ce's own bound on that parameter - "ce's bound on Y must satisfy target's bound on T". An
unbounded child parameter cannot be forwarded into a bounded ancestor slot.

******************************
 Opcache, optimizer, and JIT
******************************

Opcache persistence walks the side tables from ``zend_persist_generic_type_table``: scalar slots
(``return_type``, ``extends``) get duped into SHM via ``zend_shared_memdup_put_free`` and walked
by ``zend_persist_type``; hash slots go through ``zend_persist_generic_type_table_ht``. Calc-side
mirrors. ``zend_persist_attributes`` persists ``zend_attribute->generic_args``. ``zend_persist_type``
handles NAMED_WITH_ARGS values whose ``name`` is ``NULL``.

``zend_try_inline_call`` (``Zend/Optimizer/optimize_func_calls.c``) skips its inline transform
when a ``ZEND_VERIFY_GENERIC_ARGUMENTS`` opcode sits between ``INIT_FCALL`` and ``DO_FCALL``: the
verify handler dereferences ``EX(call)->func``, so NOP-ing ``INIT_FCALL`` while leaving the verify
in place would walk an unrelated frame.

Under JIT, ``ZEND_VERIFY_GENERIC_ARGUMENTS`` falls through to the interpreter. The handler is
cold, so the dispatch cost is dwarfed by the type-comparison work. Code that doesn't use
turbofish doesn't emit the opcode, and the surrounding ``INIT_*`` and ``ZEND_NEW`` JIT
specializations are unaffected.
