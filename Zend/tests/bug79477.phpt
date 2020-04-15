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
--EXPECT--
default value
