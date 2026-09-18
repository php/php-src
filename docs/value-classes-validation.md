# Value class validation

Upstream base: `13cec7c276033883e1e0ee22ea92fa1838b06dca`, PHP 8.6.0-dev.
Tested on Linux x86-64, NTS, with GCC and both debug (`-O0`) and release (`-O2`)
builds. The configuration is documented in [the implementation report](value-classes.md).

## Results

| Stage | Passed | Skipped | Expected failures | Unexpected failures |
| --- | ---: | ---: | ---: | ---: |
| Phase 1: initial parser/tokenizer/readonly run | 82 | 0 | 0 | 3 diagnostic differences, resolved |
| Phase 2: value/readonly/clone/classes suites, final run | 416 | 0 | 0 | 0 |
| Full available PHP suite, debug binary | **13,654** | **9,289** | **8** | **0** |
| Zend portion of the full run | 5,458 | 175 | 1 | 0 |
| Reflection portion of the full run | 561 | 12 | 0 | 0 |
| Tokenizer portion of the full run | 53 | 0 | 0 | 0 |
| OPcache portion of the full run | 905 | 35 | 2 | 0 |
| Release targeted regression suites | 1,033 | 12 | 0 | 0 |
| Value group, OPcache file-cache prime | 35 | 0 | 0 | 0 |
| Value group, OPcache file-cache read | 35 | 0 | 0 | 0 |
| Value group plus dedicated hot JIT test, tracing JIT | 36 | 0 | 0 | 0 |

The full run discovered 22,951 tests, tested 14 extensions and skipped 59
extensions absent from the minimal configuration. The runner executed/individually
classified 14,915 test files; extension-level skips account for the remaining
8,036 skipped tests. Its final elapsed time was 41.246 seconds with 12 workers.
Network-dependent tests were offline by default. Slow tests were not explicitly
disabled. There were no warnings, borked tests or unexpected failures in the
completed full run.

The patch adds **37 PHPT tests**: 35 in `Zend/tests/value_classes`, one OPcache
persistence test, and one hot JIT test. The OPcache test has a separate fixture.
Coverage includes promoted and normal properties, default/late initialization,
mutation and references, missing types, static properties, dynamic properties
and their attribute, direct/delayed inheritance, parents (including unresolved
parents), destructor/clone methods, trait imports and aliases, interfaces,
Reflection, cloning/clone-with, serialization/type errors, nullable/union/enum
fields, contextual identifiers, shallow readonly, and `==`/`===`/weak identity.

## Reproduction commands

Full debug run, from the repository root:

```sh
sapi/cli/php -n run-tests.php -q -j12 -n -g FAIL,BORK,WARN,LEAK \
  -W /tmp/php-value-class-results/full-debug-status.txt \
  -w /tmp/php-value-class-results/full-debug-failures.txt
```

The full run included all Zend tests; no separate shortened Zend selection was
substituted. After rebuilding without debug:

```sh
sapi/cli/php -n run-tests.php -q -j1 -n \
  Zend/tests/value_classes Zend/tests/readonly_classes Zend/tests/readonly_props \
  Zend/tests/clone tests/classes ext/reflection/tests ext/tokenizer/tests \
  ext/opcache/tests/value_class.phpt ext/opcache/tests/jit/value_class.phpt

sapi/cli/php -n run-tests.php -q -j1 -n --file-cache-prime Zend/tests/value_classes
sapi/cli/php -n run-tests.php -q -j1 -n --file-cache-use Zend/tests/value_classes

sapi/cli/php -n run-tests.php -q -j1 -n \
  -d opcache.enable_cli=1 -d opcache.file_update_protection=0 \
  -d opcache.jit_buffer_size=32M -d opcache.jit=tracing \
  -d opcache.jit_hot_func=1 -d opcache.jit_hot_loop=1 \
  Zend/tests/value_classes ext/opcache/tests/jit/value_class.phpt
```

Intermediate runs also checked tokenizer, readonly, final and Reflection after
each engine implementation stage. The final release targeted run took 12.069
seconds with one worker. Single-worker runs need no test-worker TCP socket;
parallel runs needed sandbox authorization for localhost communication.

## Exact expected failures in the full run

These tests already contain upstream `XFAIL` expectations; this patch did not
add, remove or change those expectations:

```text
tests/security/open_basedir_linkinfo.phpt
Zend/tests/inheritance/interface_constructor_prototype_002.phpt
ext/opcache/tests/gh18985.phpt
ext/opcache/tests/opt/verify_return_type.phpt
ext/spl/tests/ArrayObject/gh10519.phpt
ext/standard/tests/filters/stream_filter_register_class_coerce_consumed_by_ref_param.phpt
ext/standard/tests/filters/stream_filter_register_mock_class_filter.phpt
ext/uri/tests/whatwg/builder/basic_success_with_base.phpt
```

## Issues found and resolved during development

* `readonly_enum.phpt`, `readonly_interface.phpt`, `readonly_trait.phpt` initially
  failed because Bison omitted its expected-token list after the grammar gained
  a fifth class-modifier alternative. The declarations still fail. Only those
  diagnostic expectations were updated, as described in the main report.
* The new `dynamic_attribute.phpt` initially omitted the leading backslash in
  the engine's existing `#[\AllowDynamicProperties]` error. Its expectation was
  corrected to the established readonly diagnostic; engine behavior was not
  changed.
* Adding `isValue()` required extending
  `ext/reflection/tests/ReflectionClass_toString_001.phpt` with the actual new
  method and updating the method count, 64 to 65.
* The first OPcache fixture used an unconditional interface declaration.
  `opcache_compile_file()` early-bound it in the compiling request, so including
  the file again correctly produced `Cannot redeclare interface ValueIdentifier`.
  The fixture now uses conditional declarations, exercising the intended
  cached class-linking path without a duplicate declaration.
* Moving `T_VALUE` to the end of the token declarations preserved existing token
  numbers, but exposed an incremental-build ordering issue: the scanner object
  still used the previous generated header. A second dependency rebuild fixed
  the temporary parse error in `run-tests.php`; the final release build was
  clean and all parser/tokenizer tests pass.
* The benchmark smoke test exposed an unnecessary dependency on the disabled
  `filter` extension. Argument validation now uses core PHP operations only.

The initial clean debug build emitted re2c unreachable-rule warnings in the
unchanged INI and JSON scanners. There were no warnings from the new language
scanner rule or parser conflicts. The release build completed without warnings.

## Limits of validation

No Windows, 32-bit, ZTS, sanitizer, Valgrind or third-party extension matrix was
run. The full suite uses the minimal configured set of extensions, not every
optional PHP extension. Files under `/tmp/php-value-class-results` retain build
logs, per-stage logs/status lists and the debug binary for local inspection;
they are not required to build the patch.
