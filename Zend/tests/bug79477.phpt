--TEST--
Bug #79477: casting object into array creates references
--FILE--
<?php

class Test {
    public $prop = 'default value';
}

$obj = new Test;
$obj->{1} = null;

$arr = (array) $obj;
$arr['prop'] = 'new value';

echo $obj->prop, "\n";

?>
--EXPECTF--
Deprecated: Creation of dynamic property Test::$1 is deprecated in %s on line %d
default value
