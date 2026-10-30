--TEST--
GH-22857: Function JIT emits wrong code for FETCH_OBJ_FUNC_ARG on a property hook (SIMPLE_GET fast path)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=1205
opcache.jit_hot_func=1
--EXTENSIONS--
opcache
--FILE--
<?php
namespace Test;

interface HandlerInterface { public function noop(): void; }

final class DefaultHandler implements HandlerInterface {
    private static ?self $i = null;
    public static function getInstance(): self { return self::$i ??= new self(); }
    public function noop(): void {}
}

/* Original issue: virtual property hook read via FETCH_OBJ_FUNC_ARG under
 * function JIT. The getter frame is pushed by the SIMPLE_GET fast path in the
 * shared FETCH_OBJ_R handler, but the JIT-compiled FUNC_ARG opcode had no
 * hook-enter guard, so the argument slot was read before the getter ran.
 *
 * The assertion is deterministic: the getter sets a static flag as a side
 * effect, so we check whether the getter actually ran when the property is
 * passed through FETCH_OBJ_FUNC_ARG. This avoids the previous data-dependent
 * failure mode (relying on file_get_contents() fataling on whatever garbage
 * the unguarded JIT read happened to land on), which made the regression
 * test flaky. */
class Container {
    private static bool $getterRan = false;

    public protected(set) HandlerInterface $handler;

    public string $path {
        get => (self::$getterRan = true)
            ? self::build($this->kind, $this->id)
            : self::build($this->kind, $this->id);
    }

    protected mixed $prev = null;

    public function __construct(
        public protected(set) string $kind,
        public protected(set) string $id,
    ) {
        $this->handler = DefaultHandler::getInstance();
    }

    public static function build(string $k, string $i): string {
        return "/nonexistent/gh22857_{$k}_{$i}.dat";
    }

    public function step(): void {
        /* Unqualified namespaced-fallback call (INIT_NS_FCALL_BY_NAME) keeps
         * the FETCH_OBJ_FUNC_ARG opcode instead of letting the optimizer
         * rewrite it to FETCH_OBJ_R. @ preserves the opcode shape that
         * triggers the bug. */
        @file_get_contents($this->path);
        if (!self::$getterRan) {
            throw new \RuntimeException('getter did not run via FUNC_ARG');
        }
    }
}

$c = new Container('alpha', 'beta');
$c->step();
$c->step();
$c->step();

/* Sibling-slot variant: a preceding plain FETCH_OBJ_R primes the
 * SIMPLE_GET bit on the property cache slot; compact_literals shares the
 * slot between FETCH_OBJ_R and FETCH_OBJ_FUNC_ARG for the same property,
 * so the following FETCH_OBJ_FUNC_ARG consumes that bit and hits the
 * SIMPLE_GET fast path. Without the hook-enter guard it passes whatever
 * sits in an adjacent property slot instead of running the getter. The
 * flag is reset after the priming FETCH_OBJ_R so the assertion reflects
 * only whether the getter ran during the FETCH_OBJ_FUNC_ARG read. */
class Container2 {
    private static bool $getterRan = false;

    public protected(set) HandlerInterface $handler;

    public string $path {
        get => (self::$getterRan = true)
            ? self::build($this->kind, $this->id)
            : self::build($this->kind, $this->id);
    }

    protected mixed $prev = null;

    public function __construct(
        public protected(set) string $kind,
        public protected(set) string $id,
    ) {
        $this->handler = DefaultHandler::getInstance();
    }

    public static function build(string $k, string $i): string {
        return "/nonexistent/gh22857b_{$k}_{$i}.dat";
    }

    public function step(): void {
        $this->prev = $this->path;   // FETCH_OBJ_R primes SIMPLE_GET on shared slot
        self::$getterRan = false;     // reset; flag now reflects only the FUNC_ARG read below
        @file_get_contents($this->path); // FETCH_OBJ_FUNC_ARG consumes the primed bit
        if (!self::$getterRan) {
            throw new \RuntimeException('sibling-slot variant: getter did not run via FUNC_ARG');
        }
    }
}

$c2 = new Container2('alpha', 'beta');
$c2->step();
$c2->step();
$c2->step();

echo "OK\n";
?>
--EXPECT--
OK
