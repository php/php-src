--TEST--
Virtual property hook read via FETCH_OBJ_FUNC_ARG must not prime SIMPLE_GET (function JIT)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=1205
--EXTENSIONS--
opcache
--FILE--
<?php

namespace Regression;

interface HandlerInterface { public function noop(): void; }

final class DefaultHandler implements HandlerInterface {
    private static ?self $i = null;
    public static function getInstance(): self { return self::$i ??= new self(); }
    public function noop(): void {}
}

class Container {
    public protected(set) HandlerInterface $handler;

    public string $path {
        get => self::build($this->kind, $this->id);
    }

    protected mixed $prev = null;

    public function __construct(
        public protected(set) string $kind,
        public protected(set) string $id,
    ) {
        $this->handler = DefaultHandler::getInstance();
    }

    public static function build(string $k, string $i): string {
        return "/nonexistent/regression_{$k}_{$i}.dat";
    }

    /* Reads a virtual property hook directly as arg #1 of an unqualified
     * (namespace-fallback) global builtin. The emitted opcode chain is
     *
     *   INIT_NS_FCALL_BY_NAME "Regression\\file_get_contents"
     *   CHECK_FUNC_ARG        1
     *   FETCH_OBJ_FUNC_ARG    THIS, "path"    -> tmp
     *   SEND_FUNC_ARG         tmp, 1
     *   DO_FCALL_BY_NAME
     *
     * FETCH_OBJ_FUNC_ARG dispatches into the FETCH_OBJ_R handler for
     * by-value arguments while keeping opline pointing at the FUNC_ARG
     * opcode. If zend_std_read_property() primes the SIMPLE_GET
     * property-hook cache-slot bit under such an opline, subsequent
     * FETCH_OBJ_FUNC_ARG dispatches take the SIMPLE_GET fast path in
     * zend_vm_def.h -- which pushes a hook call frame and returns with
     * ZEND_VM_ENTER_BIT set. JIT function mode dispatches FETCH_OBJ_FUNC_ARG
     * via the generic zend_jit_handler path in zend_jit.c which, unlike
     * the specialised FETCH_OBJ_R handler, has no "if hook entered, exit
     * to VM" guard and continues into JIT-compiled SEND_FUNC_ARG code
     * before the hook has actually run. The pending argument slot then
     * holds an adjacent property's raw bytes, typically surfacing as
     *
     *   ValueError:  file_get_contents(): Argument #1 ($filename) must
     *                not contain any null bytes
     *   TypeError:   expected string, <adjacent class> given
     *   zend_mm_heap corrupted (SIGABRT)
     *
     * All three symptoms have the same root cause. */
    public function step(): void {
        // Namespace-fallback, by-value string arg -> FETCH_OBJ_FUNC_ARG.
        // Wrapped in @ to match the real-world observed shape.
        $r = @file_get_contents($this->path);
        if ($r !== false) {
            throw new \RuntimeException('unexpected non-false return');
        }
    }
}

$c = new Container('alpha', 'beta');

for ($i = 0; $i < 200; $i++) {
    try {
        $c->step();
    } catch (\Throwable $e) {
        printf("iter=%d threw %s: %s\n", $i, $e::class, $e->getMessage());
        exit(1);
    }
}

echo "OK\n";

?>
--EXPECT--
OK
