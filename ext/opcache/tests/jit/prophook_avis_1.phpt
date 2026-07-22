--TEST--
JIT: virtual property hook must be invoked when a preceding asymmetric-visibility property lives in the same class - jit 1235
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1235
opcache.jit_buffer_size=16M
opcache.jit_hot_func=1
opcache.jit_hot_loop=1
opcache.jit_hot_return=1
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

    public function read(): string
    {
        return $this->label;
    }
}

$c = new Container('alpha', 'beta');

/* Warm-up: trip tracing JIT thresholds for Container::read(). */
for ($i = 0; $i < 100000; $i++) {
    $r = $c->read();
}

var_dump($r);
var_dump(gettype($r));
?>
--EXPECT--
string(21) "container[alpha:beta]"
string(6) "string"
