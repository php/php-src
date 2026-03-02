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
// Note: since libxml2 commit aca16fb3d45e0b2c45364ffc1cea8eb4abaca87d this only outputs 1 warning. This seems intentional.
// Easiest workaround is silencing the warnings
@$xpath->query('//*[foo:test()]');
$xpath->registerPhpFunctionNS('urn:foo', 'test', [new Test, 'test']);
$xpath->query('//*[foo:test()]');

?>
--EXPECT--
=== First run ===
=== Reconstruct ===
destruct
=== Second run ===
test
destruct
