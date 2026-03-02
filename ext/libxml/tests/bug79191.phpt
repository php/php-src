--TEST--
Bug #79191 (Error in SoapClient ctor disables DOMDocument::save())
--EXTENSIONS--
soap
dom
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
