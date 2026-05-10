--TEST--
JIT correctness: parent and substituted child preserve their own contracts when both are hot
--INI--
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
opcache.protect_memory=1
--FILE--
<?php
class Box<T : int|string> {
    public function take(T $v): void {}
}

class IntBox extends Box<int> {}
class StringBox extends Box<string> {}

(new IntBox())->take(1);
(new StringBox())->take("x");
(new Box())->take(1);
(new Box())->take("x");

try {
    (new IntBox())->take([1]);
} catch (TypeError $e) {
    echo "intbox: ", $e->getMessage(), "\n";
}

try {
    (new StringBox())->take([1]);
} catch (TypeError $e) {
    echo "stringbox: ", $e->getMessage(), "\n";
}

(new Box())->take(42);
(new Box())->take("hello");
echo "parent accepts both\n";
?>
--EXPECTF--
intbox: Box::take(): Argument #1 ($v) must be of type int, array given, called in %s on line %d
stringbox: Box::take(): Argument #1 ($v) must be of type string, array given, called in %s on line %d
parent accepts both
