--TEST--
Bug #70662: Duplicate array key via undefined index error handler
--FILE--
<?php

$a = [];
set_error_handler(function() use(&$a) {
    $a['b'] = 2;
});
$a['b'] += 1;
var_dump($a);

?>
--EXPECT--
array(1) {
  ["b"]=>
  int(1)
}
