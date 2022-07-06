--TEST--
Overwrite reference in untyped property
--FILE--
<?php
class Test {
    public $prop;
}
$s = <<<'STR'
O:4:"Test":2:{s:4:"prop";R:1;s:4:"prop";i:0;}
STR;
var_dump(unserialize($s));
?>
--EXPECT--
object(Test)#1 (1) {
  ["prop"]=>
  int(0)
}
