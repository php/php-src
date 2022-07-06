--TEST--
Closure 040: Rebinding closures, bad arguments
--FILE--
<?php

class A {
    private $x;
    private static $xs = 10;

    public function __construct($v) {
        $this->x = $v;
    }

    public function getIncrementor() {
        return function() { return ++$this->x; };
    }
    public function getStaticIncrementor() {
        return static function() { return ++static::$xs; };
    }
}

$a = new A(20);

$ca = $a->getIncrementor();
$cas = $a->getStaticIncrementor();

try {
    $ca->bindTo($a, array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$cas->bindTo($a, 'A');

?>
--EXPECTF--
Closure::bindTo(): Argument #2 ($newScope) must be of type object|string|null, array given

Warning: Cannot bind an instance to a static closure in %s on line %d
