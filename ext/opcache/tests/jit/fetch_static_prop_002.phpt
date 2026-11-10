--TEST--
FETCH_STATIC_PROP_* with static:: must respect the called scope
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--EXTENSIONS--
opcache
--FILE--
<?php

trait T {
    public static $t = 'trait';

    public function getT() {
        return static::$t;
    }
}

class A {
    use T;
    public static $v = 'A';
    public static array $arr = [];
    public static int $typed;

    public static function getStatic() {
        return static::$v;
    }

    public function getInstance() {
        return static::$v;
    }

    public function getClosure() {
        return function () {
            return static::$v;
        };
    }

    public static function getStaticClosure() {
        return static fn () => static::$v;
    }

    public static function push($x) {
        static::$arr[] = $x;
    }

    public static function has() {
        return isset(static::$v);
    }

    public static function getTyped() {
        return static::$typed;
    }
}

// Used for the shared cache slot testing
class B extends A {
    public static $v = 'B';
    public static array $arr = [];
    public static $t = 'traitB';
}

$out = [];
for ($i = 0; $i < 8; $i++) {
    $o = $i % 2 ? new B : new A;
    $c = $i % 2 ? 'B' : 'A';
    $out[] = $c::getStatic() . $o->getInstance() . ($o->getClosure())() . ($c::getStaticClosure())() . $o->getT() . (int) $c::has();
    $c::push($i);
}
echo implode(',', $out), "\n";
print_r(A::$arr);
echo "\n";
print_r(B::$arr);
echo "\n";

for ($i = 0; $i < 2; $i++) {
    try {
        A::getTyped();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
A::$typed = 42;
var_dump(A::getTyped(), B::getTyped());

?>
--EXPECT--
AAAAtrait1,BBBBtraitB1,AAAAtrait1,BBBBtraitB1,AAAAtrait1,BBBBtraitB1,AAAAtrait1,BBBBtraitB1
Array
(
    [0] => 0
    [1] => 2
    [2] => 4
    [3] => 6
)

Array
(
    [0] => 1
    [1] => 3
    [2] => 5
    [3] => 7
)

Typed static property A::$typed must not be accessed before initialization
Typed static property A::$typed must not be accessed before initialization
int(42)
int(42)
