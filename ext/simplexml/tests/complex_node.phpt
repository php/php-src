--TEST--
Test that it's not possible to assign a complex type to nodes
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml = simplexml_load_string(<<<EOF
<?xml version="1.0"?>
<root>
 <child>Hello</child>
</root>
EOF);

try {
    $xml->child = new stdClass();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
It's not possible to assign a complex type to nodes, array given
