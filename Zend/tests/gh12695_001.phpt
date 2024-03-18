--TEST--
Coalesce IS fetch should repeat dynamic property check after __isset
--FILE--
<?php
#[AllowDynamicProperties]
class A {
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
echo $a->foo ?? 234;
?>
--EXPECT--
__isset
123
