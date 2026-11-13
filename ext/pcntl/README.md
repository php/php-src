# Memory re-keying after fork

`pcntl_fork(bool $rekey_memory = true)`,
`pcntl_rfork(int $flags, int $signal = 0, bool $rekey_memory = true)` and
`pcntl_forkx(int $flags, bool $rekey_memory = true)` refresh the allocator's
shadow key in the child by default. This requires traversing and writing the
inherited free lists and cached chunk list. A fragmented heap can therefore
cause substantial child startup time and copy-on-write memory overhead.

Passing `rekey_memory: false` skips that traversal and retains the inherited
shadow key. Allocator links remain protected, but the parent and children share
the key: disclosure of that key in one process compromises this protection in
other processes that retain it. Callers must weigh this reduced isolation
against the startup and memory savings. The default preserves per-child key
isolation.

Both modes reset the allocator's random generator state so future key refreshes
use fresh randomness. Both also update allocator process bookkeeping and perform
the usual child initialization, including timer initialization, stat and realpath
cache clearing, and extension child initialization hooks. The setting applies to
one fork; subsequent forks use their own argument, defaulting to `true`.
