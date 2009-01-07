--TEST--
Bug #47027 (var_export doesn't show numeric indices on ArrayObject)
--FILE--
<?php
$ao = new ArrayObject(array (2 => "foo", "bar" => "baz"));
var_export ($ao);
?>
--EXPECT--
ArrayObject::__set_state(array(
   2 => 'foo',
   'bar' => 'baz',
))
