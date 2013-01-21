--TEST--
Accessors can be re-declared with a plain property
--FILE--
<?php

class Base {
    public $foo {
        get { return 'test'; }
        set { echo "setting \$foo to $value\n"; }
    }
}

class Child_Overriding extends Base {
    public $foo;
}

class Child_Inheriting extends Base {
    public $foo {}
}

$base = new Base;
$base->foo = 'bar';
var_dump($base->foo);

$childO = new Child_Overriding;
$childO->foo = 'bar';
var_dump($childO->foo);

$childI = new Child_Inheriting;
$childI->foo = 'bar';
var_dump($childI->foo);

?>
--EXPECT--
setting $foo to bar
string(4) "test"
string(3) "bar"
setting $foo to bar
string(4) "test"
