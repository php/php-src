--TEST--
GH-15654 (Signed integer overflow in ext/dom/nodelist.c)
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die('skip 64-bit only');
?>
--FILE--
<?php
define("MAX_64Bit", 9223372036854775807);
define("MAX_32Bit", 2147483647);
define("MIN_64Bit", -9223372036854775807 - 1);
define("MIN_32Bit", -2147483647 - 1);
$longVals = array(
    0, MAX_64Bit, MIN_64Bit, MAX_32Bit, MIN_32Bit, MAX_64Bit - MAX_32Bit, MIN_64Bit - MIN_32Bit,
);
$dom = new DOMDocument;
$dom->loadXML('<root><a/><b/><c/></root>');
$children = $dom->documentElement->childNodes;
foreach ($longVals as $value) {
    var_dump($children[$value]?->nodeName);
}
?>
--EXPECT--
string(1) "a"
NULL
NULL
NULL
NULL
NULL
NULL
