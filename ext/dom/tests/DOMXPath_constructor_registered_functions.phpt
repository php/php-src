--TEST--
DOMXPath: Calling __construct() again when functions were already registered
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (!class_exists('DOMXPath')) die('skip no xpath support');
?>
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML('<root/>');

class Test {
    public function __destruct() {
        echo "destruct\n";
    }

    public function test() {
        echo "test\n";
    }
}

echo "=== First run ===\n";

$xpath = new DOMXPath($dom);
$xpath->registerNamespace('foo', 'urn:foo');
$xpath->registerPhpFunctionNS('urn:foo', 'test', [new Test, 'test']);

echo "=== Reconstruct ===\n";

$xpath->__construct($dom, true);

echo "=== Second run ===\n";

$xpath->registerNamespace('foo', 'urn:foo');
$xpath->query('//*[foo:test()]');
$xpath->registerPhpFunctionNS('urn:foo', 'test', [new Test, 'test']);
$xpath->query('//*[foo:test()]');

?>
--EXPECTF--
=== First run ===
=== Reconstruct ===
destruct
=== Second run ===

Warning: DOMXPath::query(): xmlXPathCompOpEval: function test not found in %s on line %d

Warning: DOMXPath::query(): Unregistered function in %s on line %d
test
destruct
