--TEST--
Overwriting a typed property that is not yet a reference
--FILE--
<?php

class Test {
    public ?Test $prop;
}
$s = <<<'STR'
O:4:"Test":2:{s:4:"prop";N;s:4:"prop";O:4:"Test":1:{s:4:"prop";R:2;}}
STR;
var_dump(unserialize($s));

?>
--EXPECT--
object(Test)#1 (1) {
  ["prop"]=>
  &object(Test)#2 (1) {
    ["prop"]=>
    *RECURSION*
  }
}
