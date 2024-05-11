--TEST--
Bug #79971 (special character is breaking the path in xml function)
--EXTENSIONS--
dom
--FILE--
<?php
$imp = new DOMImplementation;
if (PHP_OS_FAMILY === 'Windows') {
    $path = '/' . str_replace('\\', '/', __DIR__);
} else {
    $path = __DIR__;
}
$uri = "file://$path/bug79971_2.xml";
var_dump($imp->createDocumentType("$uri%00foo"));
?>
--EXPECTF--
Warning: DOMImplementation::createDocumentType(): URI must not contain percent-encoded NUL bytes in %s on line %d
bool(false)
