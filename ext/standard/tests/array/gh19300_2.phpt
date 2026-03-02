--TEST--
GH-19300 (Nested array_multisort invocation with error breaks) - error variation
--FILE--
<?php

function error_handle($level, $message, $file = '', $line = 0){
    try {
        array_multisort($a, SORT_ASC); // Trigger multisort abort
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
set_error_handler('error_handle');

$inputs = [
    new stdClass,
    new stdClass,
    new stdClass,
];

var_dump(array_multisort($inputs, SORT_NUMERIC));
var_dump($inputs);
?>
--EXPECT--
array_multisort(): Argument #1 ($array) must be an array or a sort flag
array_multisort(): Argument #1 ($array) must be an array or a sort flag
array_multisort(): Argument #1 ($array) must be an array or a sort flag
array_multisort(): Argument #1 ($array) must be an array or a sort flag
bool(true)
array(3) {
  [0]=>
  object(stdClass)#1 (0) {
  }
  [1]=>
  object(stdClass)#2 (0) {
  }
  [2]=>
  object(stdClass)#3 (0) {
  }
}
