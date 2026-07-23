--TEST--
Virtual property hook read via FETCH_OBJ_FUNC_ARG must invoke the getter (function JIT)
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

    /* Passes a virtual property hook directly as the first argument of an
     * unqualified (namespace-fallback) global function. That fallback path
     * emits INIT_NS_FCALL_BY_NAME + CHECK_FUNC_ARG + FETCH_OBJ_FUNC_ARG,
     * and the FETCH_OBJ_FUNC_ARG handler tail-calls into the FETCH_OBJ_R
     * handler for by-value arguments.
     *
     * zend_std_read_property() must not prime the SIMPLE_GET property-hook
     * cache-slot bit while the currently-executing opline is anything
     * other than a plain ZEND_FETCH_OBJ_R. If it does, subsequent hook
     * reads (potentially via a JIT-compiled fast path for FUNC_ARG that
     * has no hook-enter check) go through the SIMPLE_GET path with a
     * mismatched opline and read garbage from an adjacent property slot,
     * typically surfacing as ValueError "must not contain any null bytes"
     * or a TypeError referencing the neighbour's class.
     *
     * Also exercises the same fetch under @-silencing (BEGIN_SILENCE /
     * END_SILENCE) which is the exact shape observed in the wild. */
    public function step(): void {
        // First: a bare FETCH_OBJ_FUNC_ARG under a namespaced call.
        $len = strlen($this->path);
        if ($len === 0) {
            throw new \RuntimeException('empty path from hook');
        }
        // Second: a silenced namespaced call with the hook as arg #1.
        // file_get_contents() is a by-value string parameter so the
        // FETCH_OBJ_FUNC_ARG dispatches into the FETCH_OBJ_R handler.
        $r = @file_get_contents($this->path);
        // The file does not exist, so a false return with a warning is
        // expected. The important thing is that no ValueError / TypeError
        // / heap corruption occurs while producing the argument.
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
