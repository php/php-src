--TEST--
Overwriting an undeclared property with protected mangling
--FILE--
<?php
class Test {
    // We need at least one declared property, even though we don't use it.
    public $foo;
}

$str = <<<STR
O:4:"Test":2:{s:4:"\0*\0x";N;s:4:"\0*\0x";N;}
STR;
var_dump(unserialize($str));
?>
--EXPECTF--
Deprecated: Creation of dynamic property Test::$x is deprecated in %s on line %d
object(Test)#1 (2) {
  ["foo"]=>
  NULL
  ["x":protected]=>
  NULL
}
