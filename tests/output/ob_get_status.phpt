--TEST--
ob_get_status() function basic test
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php
ob_start();
$status = ob_get_status();
ob_end_clean();
var_dump($status);

ob_start();
$status = ob_get_status(false);
ob_end_clean();
var_dump($status);

ob_start();
$status = ob_get_status(true);
ob_end_clean();
var_dump($status);
?>
--EXPECT--
array(7) {
  ["name"]=>
  string(22) "default output handler"
  ["type"]=>
  int(0)
  ["flags"]=>
  int(112)
  ["level"]=>
  int(0)
  ["chunk_size"]=>
  int(0)
  ["buffer_size"]=>
  int(16384)
  ["buffer_used"]=>
  int(0)
}
array(7) {
  ["name"]=>
  string(22) "default output handler"
  ["type"]=>
  int(0)
  ["flags"]=>
  int(112)
  ["level"]=>
  int(0)
  ["chunk_size"]=>
  int(0)
  ["buffer_size"]=>
  int(16384)
  ["buffer_used"]=>
  int(0)
}
array(1) {
  [0]=>
  array(7) {
    ["name"]=>
    string(22) "default output handler"
    ["type"]=>
    int(0)
    ["flags"]=>
    int(112)
    ["level"]=>
    int(0)
    ["chunk_size"]=>
    int(0)
    ["buffer_size"]=>
    int(16384)
    ["buffer_used"]=>
    int(0)
  }
}
