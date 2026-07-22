--TEST--
JIT: virtual property hook must be invoked when a preceding asymmetric-visibility property lives in the same class - jit 1205 (function JIT)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1205
opcache.jit_buffer_size=16M
--EXTENSIONS--
opcache
--FILE--
<?php

interface HandlerInterface {
    public function name(): string;
}

class DefaultHandler implements HandlerInterface {
    public function name(): string { return 'default'; }
}

class Container
{
    public protected(set) HandlerInterface $handler;

    public string $label {
        get => "container[{$this->kind}:{$this->key}]";
    }

    public function __construct(
        public protected(set) string $kind,
        public protected(set) string $key
    ) {
        $this->handler = new DefaultHandler();
    }

    /* Method-scoped access: op_array->scope == Container and opline
     * op1_type == IS_UNUSED for $this->label, exercising the JIT code path
     * that must emit an offset check for hooked properties. */
    public function read(): string
    {
        return $this->label;
    }
}

/* Also exercise the non-$this path, where op1 is a variable holding an
 * instance of Container. */
function readLabel(Container $c): string {
    return $c->label;
}

$c = new Container('alpha', 'beta');

var_dump($c->read());
var_dump(readLabel($c));
var_dump($c->handler->name());
?>
--EXPECT--
string(21) "container[alpha:beta]"
string(21) "container[alpha:beta]"
string(7) "default"
