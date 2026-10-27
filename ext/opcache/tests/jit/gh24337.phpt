--TEST--
GH-24337 (JIT: segfault in zend_get_executed_lineno when decrementing a non-numeric string property)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.jit_hot_loop=2
opcache.jit_hot_func=2
opcache.jit_hot_return=2
opcache.jit_hot_side_exit=1
--EXTENSIONS--
opcache
--FILE--
<?php
set_error_handler(function ($errno, $errstr, $errfile, $errline) {
    echo "$errstr on line $errline\n";
});
class Q {
    public $pu = 0;
    function run() {
        for ($i = 0; $i < 2; $i++) {
            $this->pu--;
        }
    }
    function inc() {
        for ($i = 0; $i < 2; $i++) {
            $this->pu++;
        }
    }
}
class S extends Q {
    public $pu = 'u';
    function run() {
        parent::run();
    }
}
class B extends Q {
    public $pu = true;
    function run() {
        parent::run();
    }
}
class A extends Q {
    public $pu = [];
    function run() {
        parent::run();
    }
}
$objs = [new Q, new S, new B, new A];
for ($k = 0; $k < 4; $k++) {
    foreach ($objs as $o) {
        try {
            $o->run();
        } catch (TypeError $e) {
            echo $e->getMessage(), " on line ", $e->getLine(), "\n";
        }
        try {
            $o->inc();
        } catch (TypeError $e) {
            echo $e->getMessage(), " on line ", $e->getLine(), "\n";
        }
    }
}
var_dump($objs[0]->pu, $objs[1]->pu, $objs[2]->pu, $objs[3]->pu);
?>
--EXPECT--
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
Increment on non-numeric string is deprecated, use str_increment() instead on line 14
Increment on non-numeric string is deprecated, use str_increment() instead on line 14
Decrement on type bool has no effect, this will change in the next major version of PHP on line 9
Decrement on type bool has no effect, this will change in the next major version of PHP on line 9
Increment on type bool has no effect, this will change in the next major version of PHP on line 14
Increment on type bool has no effect, this will change in the next major version of PHP on line 14
Cannot decrement array on line 9
Cannot increment array on line 14
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
Increment on non-numeric string is deprecated, use str_increment() instead on line 14
Increment on non-numeric string is deprecated, use str_increment() instead on line 14
Decrement on type bool has no effect, this will change in the next major version of PHP on line 9
Decrement on type bool has no effect, this will change in the next major version of PHP on line 9
Increment on type bool has no effect, this will change in the next major version of PHP on line 14
Increment on type bool has no effect, this will change in the next major version of PHP on line 14
Cannot decrement array on line 9
Cannot increment array on line 14
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
Increment on non-numeric string is deprecated, use str_increment() instead on line 14
Increment on non-numeric string is deprecated, use str_increment() instead on line 14
Decrement on type bool has no effect, this will change in the next major version of PHP on line 9
Decrement on type bool has no effect, this will change in the next major version of PHP on line 9
Increment on type bool has no effect, this will change in the next major version of PHP on line 14
Increment on type bool has no effect, this will change in the next major version of PHP on line 14
Cannot decrement array on line 9
Cannot increment array on line 14
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
Increment on non-numeric string is deprecated, use str_increment() instead on line 14
Increment on non-numeric string is deprecated, use str_increment() instead on line 14
Decrement on type bool has no effect, this will change in the next major version of PHP on line 9
Decrement on type bool has no effect, this will change in the next major version of PHP on line 9
Increment on type bool has no effect, this will change in the next major version of PHP on line 14
Increment on type bool has no effect, this will change in the next major version of PHP on line 14
Cannot decrement array on line 9
Cannot increment array on line 14
int(0)
string(2) "ac"
bool(true)
array(0) {
}
