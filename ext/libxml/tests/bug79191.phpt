--TEST--
Bug #79191 (Error in SoapClient ctor disables DOMDocument::save())
--SKIPIF--
<?php
if (!extension_loaded('soap')) die('skip soap extension not available');
if (!extension_loaded('dom')) die('dom extension not available');
?>
--FILE--
<?php
try {
    new \SoapClient('does-not-exist.wsdl');
} catch (Throwable $t) {
}

$dom = new DOMDocument;
$dom->loadxml('<?xml version="1.0" ?><root />');
var_dump($dom->save(__DIR__ . '/bug79191.xml'));
?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug79191.xml');
?>
--EXPECTF--
int(%d)
