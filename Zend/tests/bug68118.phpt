--TEST--
Bug #68118: $a->foo .= 'test'; can leave $a->foo undefined
--FILE--
<?php

set_error_handler(function() {
    $obj = new StdClass;
    $obj->test = 'meow';
    return true;
});

$a = new StdClass;
$a->undefined .= 'test';
var_dump($a);

?>
--EXPECT--
object(StdClass)#2 (1) {
  ["undefined"]=>
  string(4) "test"
}
