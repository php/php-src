--TEST--
Constants in default values of properties
--FILE--
<?php
declare(strict_types=1);

define("FOO", 5);

class A {
    public int $foo = FOO;
}

class B {
    public string $foo = FOO;
}

$o = new A();
var_dump($o->foo);

for ($i = 0; $i < 2; $i++) {
    try {
        $o = new B();
        var_dump($o->foo);
    } catch (Throwable $e) {
        echo $e->getMessage() . "\n";
    }
}
?>
--EXPECT--
int(5)
Cannot assign int to property B::$foo of type string
Cannot assign int to property B::$foo of type string
