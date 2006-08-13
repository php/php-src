--TEST--
Bug #38438 (DOMNodeList->item(0) segfault on empty NodeList)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$list = new DOMNodeList();
var_dump($list->item(0));
echo "OK\n";
?>
--EXPECT--
NULL
OK
