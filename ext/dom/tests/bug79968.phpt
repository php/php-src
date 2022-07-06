--TEST--
dom: Bug #79968 - Crash when calling before without valid hierachy
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$cdata = new DOMText;

try {
    $cdata->before("string");
} catch (DOMException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Hierarchy Request Error
