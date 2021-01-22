--TEST--
Overwriting a typed property reference
--FILE--
<?php

class Test {
    public ?object $prop;
}
$s = <<<'STR'
O:4:"Test":2:{s:4:"prop";R:1;s:4:"prop";N;}}
STR;
var_dump(unserialize($s));

?>
--EXPECT--
object(Test)#1 (1) {
  ["prop"]=>
  NULL
}
