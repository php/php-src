# Value class baseline

Run with the custom PHP binary, built with `--disable-debug`:

```sh
sapi/cli/php -n benchmarks/value_classes/run.php 1000000 7 200000 > benchmark.json
```

Arguments are timed iterations, repetitions (at least three), and live instances
for the memory measurement. The driver needs only the extensions in the minimal
build. Each sample uses a fresh PHP process with `-n`, OPcache CLI disabled and
JIT disabled. Class order rotates between repetitions. The timing workers warm
up for up to 100,000 iterations before measuring with `hrtime(true)`.

`classes.inc` defines exactly the three requested shapes: a final ordinary class
with a readonly string property, a readonly class, and a value class, each with
the same constructor and `toString()` method. Construction loops name classes
directly. Read and method loops use an existing instance and consume string
lengths in a checksum; all workers verify their result. Reported timings include
loop overhead and, for reads/calls, `strlen()` and checksum accumulation.
Construction includes releasing the preceding instance.

The memory workload retains instances in a preallocated packed array. It
subtracts the array's existing storage and reports `memory_get_usage(false)`
growth, **including object-store growth**. It also records allocator page growth
with `memory_get_usage(true)`. All instances share the same literal string, so
these numbers exclude per-instance string allocation. They are neither RSS nor
a C `sizeof` measurement. Allocation pages and handle-table growth can be
stepwise.

The JSON contains raw samples, medians, minimum/maximum values and build/runtime
metadata. Timings on a shared workstation are noisy; small differences are not
evidence of a language-feature speedup. No specialized value representation is
implemented.

## Measured baseline (2026-09-18)

PHP 8.6.0-dev NTS, GCC `-O2 -DNDEBUG`, Linux x86-64, Intel Core i7-12850HX.
The driver and its children were pinned to logical CPU 2, a performance core:

```sh
taskset -c 2 sapi/cli/php -n benchmarks/value_classes/run.php 2000000 9 200000 \
  > benchmarks/value_classes/baseline.json
```

Nine samples per class/workload, 2,000,000 timed iterations per sample,
200,000 simultaneously retained instances for memory. Both OPcache and JIT were
off. Medians:

| Metric | Final ordinary + readonly property | Readonly class | Value class |
| --- | ---: | ---: | ---: |
| Construction/release, ns/op | 32.54 | 32.78 | 32.87 |
| Construction throughput, million ops/s | 30.73 | 30.51 | 30.43 |
| Property read/checksum, ns/op | 5.81 | 5.81 | 5.81 |
| Method call/checksum, ns/op | 12.41 | 12.45 | 12.44 |
| Live memory growth, bytes/instance | 66.44496 | 66.44496 | 66.44496 |
| Live memory growth, 200,000 instances, bytes | 13,288,992 | 13,288,992 | 13,288,992 |

Allocator page growth was 12,582,912 bytes in all memory samples. It can be less
than live allocation growth because preexisting allocator pages contain free
space. The live-memory figures include object-store capacity growth and exclude
the preallocated array slots.

The [raw measurements](baseline.json) retain every sample, including one ordinary
construction sample of 76.12 ns/op (its median is 32.54). None was removed. These
results show comparable object costs and **no demonstrated value-class speedup**.
They are one workstation baseline, not a statistically controlled performance
claim or a measurement of OPcache/JIT performance.

## Inspecting current optimization

```sh
sapi/cli/php -n -d opcache.enable_cli=1 \
  -d opcache.file_update_protection=0 \
  -d opcache.opt_debug_level=0x30000 \
  benchmarks/value_classes/optimizer.php
```

`optimizer.php` exercises the requested `Money` constructor example and a
constructor-free value class with a constant property default. The committed
opcode dump and design discussion in [the report](../../docs/value-classes.md)
describe the optimizer as observed, rather than assuming allocation elimination.
