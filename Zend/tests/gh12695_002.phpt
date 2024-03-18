--TEST--
Coalesce IS fetch should repeat dynamic property check after __isset
--FILE--
<?php
class A {
    public int $foo;
    public function __isset($prop) {
        echo __FUNCTION__, "\n";
        $this->$prop = 123;
        unset($GLOBALS['a']);
        return true;
    }
    public function __get($prop) {
        throw new Exception('Unreachable');
    }
}

$a = new A;
unset($a->foo);
echo $a->foo ?? 234;
?>
--EXPECT--
__isset
123
