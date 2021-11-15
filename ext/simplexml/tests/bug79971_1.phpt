--TEST--
Bug #79971 (special character is breaking the path in xml function)
--SKIPIF--
<?php
if (!extension_loaded('simplexml')) die('skip simplexml extension not available');
?>
--FILE--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    $path = '/' . str_replace('\\', '/', __DIR__);
} else {
    $path = __DIR__;
}
$uri = "file://$path/bug79971_1.xml";
var_dump(simplexml_load_file("$uri%00foo"));

$sxe = simplexml_load_file($uri);
var_dump($sxe->asXML("$uri.out%00foo"));
?>
--EXPECTF--
Warning: simplexml_load_file(): URI must not contain percent-encoded NUL bytes in %s on line %d

Warning: simplexml_load_file(): I/O warning : failed to load external entity "%s/bug79971_1.xml%%r00%rfoo" in %s on line %d
bool(false)

Warning: SimpleXMLElement::asXML(): URI must not contain percent-encoded NUL bytes in %s on line %d
bool(false)
