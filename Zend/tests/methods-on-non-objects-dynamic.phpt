--TEST--
Catch method calls on non-objects with dynamic lookups
--FILE--
<?php
set_error_handler(function($code, $message) {
  static $i= 0;
  echo 'Called #'.(++$i)."\n";
});

$arr= [null, 'method'];
var_dump($arr[0]->{$arr[1]}());

$fun= function() { return null; };
var_dump($fun()->{'method'}());

echo "Alive\n";
?>
--EXPECTF--
Called #1
NULL
Called #2
NULL
Alive
