--TEST--
Calling hook method with parent::$prop::get() from reflection
--FILE--
<?php

class A {
    public $prop;
}

class B extends A {
    public $prop {
        get {
            echo __FUNCTION__, "\n";
            return parent::$prop::get();
        }
        set {
            echo __FUNCTION__, "\n";
            parent::$prop::set($value);
        }
    }
}

$b = new B();
(new ReflectionProperty(B::class, 'prop'))->getHook(PropertyHookType::Set)->invoke($b, 43);
var_dump((new ReflectionProperty(B::class, 'prop'))->getHook(PropertyHookType::Get)->invoke($b));

?>
--EXPECT--
$prop::set
$prop::get
int(43)
