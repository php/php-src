# Changed files

65 source, test, benchmark and documentation files are changed or added. Build outputs and local logs are ignored and are not part of the patch.

| File | Purpose |
| --- | --- |
| [Zend/tests/readonly_classes/readonly_enum.phpt](../Zend/tests/readonly_classes/readonly_enum.phpt) | Account for Bison dropping its expected-token list after a fifth modifier is added; rejection is unchanged. |
| [Zend/tests/readonly_classes/readonly_interface.phpt](../Zend/tests/readonly_classes/readonly_interface.phpt) | Account for the same expected-token-list diagnostic change; rejection is unchanged. |
| [Zend/tests/readonly_classes/readonly_trait.phpt](../Zend/tests/readonly_classes/readonly_trait.phpt) | Account for the same expected-token-list diagnostic change; rejection is unchanged. |
| [Zend/tests/value_classes/abstract.phpt](../Zend/tests/value_classes/abstract.phpt) | New PHPT: A value class cannot be abstract. |
| [Zend/tests/value_classes/abstract_reversed.phpt](../Zend/tests/value_classes/abstract_reversed.phpt) | New PHPT: A value class cannot be abstract regardless of modifier order. |
| [Zend/tests/value_classes/anonymous.phpt](../Zend/tests/value_classes/anonymous.phpt) | New PHPT: Anonymous value classes are outside the MVP. |
| [Zend/tests/value_classes/basic.phpt](../Zend/tests/value_classes/basic.phpt) | New PHPT: Value classes support construction, promoted properties and methods. |
| [Zend/tests/value_classes/clone.phpt](../Zend/tests/value_classes/clone.phpt) | New PHPT: Value classes inherit readonly cloning and clone-with behavior. |
| [Zend/tests/value_classes/clone_method.phpt](../Zend/tests/value_classes/clone_method.phpt) | New PHPT: A value class cannot declare a clone method. |
| [Zend/tests/value_classes/defaults_and_initialization.phpt](../Zend/tests/value_classes/defaults_and_initialization.phpt) | New PHPT: Value classes inherit readonly defaults and delayed initialization. |
| [Zend/tests/value_classes/destructor.phpt](../Zend/tests/value_classes/destructor.phpt) | New PHPT: A value class cannot declare a destructor. |
| [Zend/tests/value_classes/duplicate_modifier.phpt](../Zend/tests/value_classes/duplicate_modifier.phpt) | New PHPT: The value class modifier cannot be repeated. |
| [Zend/tests/value_classes/dynamic_attribute.phpt](../Zend/tests/value_classes/dynamic_attribute.phpt) | New PHPT: AllowDynamicProperties cannot override value class semantics. |
| [Zend/tests/value_classes/dynamic_properties.phpt](../Zend/tests/value_classes/dynamic_properties.phpt) | New PHPT: Value classes cannot have dynamic properties. |
| [Zend/tests/value_classes/equality.phpt](../Zend/tests/value_classes/equality.phpt) | New PHPT: Value classes preserve object equality and identity semantics. |
| [Zend/tests/value_classes/extend_value.phpt](../Zend/tests/value_classes/extend_value.phpt) | New PHPT: A value class is implicitly final. |
| [Zend/tests/value_classes/extend_value_delayed.phpt](../Zend/tests/value_classes/extend_value_delayed.phpt) | New PHPT: Finality is checked when a value class is linked at runtime. |
| [Zend/tests/value_classes/extends.phpt](../Zend/tests/value_classes/extends.phpt) | New PHPT: A value class cannot extend an ordinary class. |
| [Zend/tests/value_classes/extends_readonly.phpt](../Zend/tests/value_classes/extends_readonly.phpt) | New PHPT: A value class cannot extend a readonly class. |
| [Zend/tests/value_classes/extends_unresolved.phpt](../Zend/tests/value_classes/extends_unresolved.phpt) | New PHPT: A value class rejects a parent before autoloading. |
| [Zend/tests/value_classes/interfaces.phpt](../Zend/tests/value_classes/interfaces.phpt) | New PHPT: Value classes implement interfaces, including Stringable and property contracts. |
| [Zend/tests/value_classes/missing_promoted_type.phpt](../Zend/tests/value_classes/missing_promoted_type.phpt) | New PHPT: Promoted value class properties require a declared type. |
| [Zend/tests/value_classes/missing_type.phpt](../Zend/tests/value_classes/missing_type.phpt) | New PHPT: Value class properties require a declared type. |
| [Zend/tests/value_classes/mutation.phpt](../Zend/tests/value_classes/mutation.phpt) | New PHPT: Value class properties reject direct and indirect modification and references. |
| [Zend/tests/value_classes/property_hook.phpt](../Zend/tests/value_classes/property_hook.phpt) | New PHPT: Value classes inherit the restriction on readonly property hooks. |
| [Zend/tests/value_classes/reflection.phpt](../Zend/tests/value_classes/reflection.phpt) | New PHPT: ReflectionClass and ReflectionObject expose value classes. |
| [Zend/tests/value_classes/serialization.phpt](../Zend/tests/value_classes/serialization.phpt) | New PHPT: Value classes retain ordinary readonly object serialization. |
| [Zend/tests/value_classes/shallow_readonly.phpt](../Zend/tests/value_classes/shallow_readonly.phpt) | New PHPT: Value class readonly semantics do not recursively freeze objects or array references. |
| [Zend/tests/value_classes/static_property.phpt](../Zend/tests/value_classes/static_property.phpt) | New PHPT: Value classes cannot declare static properties. |
| [Zend/tests/value_classes/syntax.phpt](../Zend/tests/value_classes/syntax.phpt) | New PHPT: Value is a contextual, case insensitive class modifier. |
| [Zend/tests/value_classes/tokenizer.phpt](../Zend/tests/value_classes/tokenizer.phpt) | New PHPT: Value is tokenized as a modifier only before a class declaration. |
| [Zend/tests/value_classes/trait_clone.phpt](../Zend/tests/value_classes/trait_clone.phpt) | New PHPT: Traits cannot introduce a clone method into a value class. |
| [Zend/tests/value_classes/trait_clone_alias.phpt](../Zend/tests/value_classes/trait_clone_alias.phpt) | New PHPT: Trait aliases cannot introduce a clone method into a value class. |
| [Zend/tests/value_classes/trait_destructor.phpt](../Zend/tests/value_classes/trait_destructor.phpt) | New PHPT: Traits cannot introduce a destructor into a value class. |
| [Zend/tests/value_classes/trait_destructor_alias.phpt](../Zend/tests/value_classes/trait_destructor_alias.phpt) | New PHPT: Trait aliases cannot introduce a destructor into a value class. |
| [Zend/tests/value_classes/trait_mutable_property.phpt](../Zend/tests/value_classes/trait_mutable_property.phpt) | New PHPT: Value classes retain readonly trait compatibility checks. |
| [Zend/tests/value_classes/traits.phpt](../Zend/tests/value_classes/traits.phpt) | New PHPT: Value classes support readonly-compatible trait properties and methods. |
| [Zend/tests/value_classes/types.phpt](../Zend/tests/value_classes/types.phpt) | New PHPT: Value class properties support nullable, union, enum and value class types. |
| [Zend/zend_API.c](../Zend/zend_API.c) | Reject destructor/clone methods at the shared direct/trait magic-method registration point. |
| [Zend/zend_ast.c](../Zend/zend_ast.c) | Print the value modifier when exporting a class AST. |
| [Zend/zend_compile.c](../Zend/zend_compile.c) | Validate modifier combinations and parent prohibition; add implicit final/readonly/no-dynamic flags. |
| [Zend/zend_compile.h](../Zend/zend_compile.h) | Define the verified free class flag at unsigned bit 31. |
| [Zend/zend_language_parser.y](../Zend/zend_language_parser.y) | Add T_VALUE and accept it through class modifiers; append the token without renumbering existing tokens. |
| [Zend/zend_language_scanner.l](../Zend/zend_language_scanner.l) | Recognize value contextually before class modifier lists, preserving identifiers elsewhere. |
| [benchmarks/value_classes/README.md](../benchmarks/value_classes/README.md) | Document benchmark reproduction, scope, measurements and caveats. |
| [benchmarks/value_classes/baseline.json](../benchmarks/value_classes/baseline.json) | Retain all 108 measured samples and their summaries from the release binary. |
| [benchmarks/value_classes/classes.inc](../benchmarks/value_classes/classes.inc) | Define the three requested object shapes with identical payload and methods. |
| [benchmarks/value_classes/optimizer-opcodes.txt](../benchmarks/value_classes/optimizer-opcodes.txt) | Capture current before/after optimizer output; only the repository root prefix is normalized. |
| [benchmarks/value_classes/optimizer.php](../benchmarks/value_classes/optimizer.php) | Probe promoted-constructor and constructor-free value objects for the design investigation. |
| [benchmarks/value_classes/run.php](../benchmarks/value_classes/run.php) | Run isolated rotating-order samples and report medians, ranges, raw samples and environment. |
| [benchmarks/value_classes/worker.php](../benchmarks/value_classes/worker.php) | Measure construction, property reads, calls and retained-object memory; validate checksums. |
| [docs/value-classes-files.md](../docs/value-classes-files.md) | Provide this complete changed-file inventory. |
| [docs/value-classes-validation.md](../docs/value-classes-validation.md) | Record commands, counts, exact expected failures, intermediate issues and validation limits. |
| [docs/value-classes.md](../docs/value-classes.md) | Document the prototype, inspected architecture, exact semantics, limits and one next optimization. |
| [ext/opcache/tests/jit/value_class.phpt](../ext/opcache/tests/jit/value_class.phpt) | New PHPT: JIT value class construction, reads and readonly guards preserve semantics. |
| [ext/opcache/tests/value_class.inc](../ext/opcache/tests/value_class.inc) | Conditional declarations for the cached interface/value-class linking test. |
| [ext/opcache/tests/value_class.phpt](../ext/opcache/tests/value_class.phpt) | New PHPT: OPcache preserves the value class flag and semantics when linking cached classes. |
| [ext/reflection/php_reflection.c](../ext/reflection/php_reflection.c) | Add isValue(), show value in class text, and use an unsigned class-flag mask. |
| [ext/reflection/php_reflection.stub.php](../ext/reflection/php_reflection.stub.php) | Declare the new boolean ReflectionClass method. |
| [ext/reflection/php_reflection_arginfo.h](../ext/reflection/php_reflection_arginfo.h) | Regenerate method registration/arginfo and stub hash. |
| [ext/reflection/php_reflection_decl.h](../ext/reflection/php_reflection_decl.h) | Regenerate the declaration header hash/guard from the updated stub. |
| [ext/reflection/tests/ReflectionClass_toString_001.phpt](../ext/reflection/tests/ReflectionClass_toString_001.phpt) | Expect the new reflected method and the 65-method count. |
| [ext/tokenizer/tokenizer_data.c](../ext/tokenizer/tokenizer_data.c) | Regenerate token_name() data with T_VALUE. |
| [ext/tokenizer/tokenizer_data.stub.php](../ext/tokenizer/tokenizer_data.stub.php) | Regenerate the public T_VALUE token constant declaration. |
| [ext/tokenizer/tokenizer_data_arginfo.h](../ext/tokenizer/tokenizer_data_arginfo.h) | Regenerate tokenizer constant registration. |
