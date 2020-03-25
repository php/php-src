--TEST--
var_export() and objects with numeric indexes properties
--FILE--
<?php
$a = (object) array (1, 3, "foo" => "bar");
var_export($a);
?>
--EXPECT--
(object) array(
   '0' => 1,
   '1' => 3,
   'foo' => 'bar',
)
