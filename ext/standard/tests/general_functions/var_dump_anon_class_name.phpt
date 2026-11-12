--TEST--
var_dump() and debug_zval_dump() print the short name of anonymous classes
--FILE--
<?php
$o = new class { public $a = 1; private $p; };
var_dump($o);
debug_zval_dump($o);
print_r($o);
echo "\n";
?>
--EXPECT--
object(class@anonymous)#1 (2) {
  ["a"]=>
  int(1)
  ["p":"class@anonymous":private]=>
  NULL
}
object(class@anonymous)#1 (2) refcount(2){
  ["a"]=>
  int(1)
  ["p":"class@anonymous":private]=>
  NULL
}
class@anonymous Object
(
    [a] => 1
    [p:class@anonymous:private] => 
)
