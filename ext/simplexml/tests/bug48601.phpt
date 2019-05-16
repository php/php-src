--TEST--
Bug #48601 (xpath() returns FALSE for legitimate query)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$sxe = simplexml_load_string('<root><node1>1</node1></root>');

$nodes = $sxe->xpath("/root/node2/@test");

if (! is_array($nodes)) {
    echo "An error occurred\n";
} else {
   echo "Result Count: " . count($nodes) . "\n";
}

?>
--EXPECT--
Result Count: 0
