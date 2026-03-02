--TEST--
Bug #35342 (isset(DOMNodeList->length) returns false)
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument();
$dom->loadXML("<root><foo>foobar</foo><foo>foobar#2</foo></root>");

$nodelist = $dom->getElementsByTagName("foo");

var_dump($nodelist->length, isset($nodelist->length), isset($nodelist->foo));
var_dump(empty($nodelist->length), empty($nodelist->foo));
?>
--EXPECT--
int(2)
bool(true)
bool(false)
bool(false)
bool(true)
