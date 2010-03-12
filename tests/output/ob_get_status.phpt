--TEST--
ob_get_status() function basic test
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php
ob_start();
$status = ob_get_status(true);
ob_end_clean();
var_dump($status);
?>
--EXPECT--
array(1) {
  [0]=>
  array(7) {
    ["chunk_size"]=>
    int(0)
    ["size"]=>
    int(40960)
    ["block_size"]=>
    int(10240)
    ["type"]=>
    int(1)
    ["status"]=>
    int(0)
    ["name"]=>
    string(22) "default output handler"
    ["del"]=>
    bool(true)
  }
}

