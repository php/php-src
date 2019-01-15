--TEST--
DOMDocument::createObjectNS() class must extend DOMElement
--CREDITS--
Adam Martinson
--SKIPIF--
<?php
require_once('skipif.inc');
if (!method_exists('DOMDocument', 'createObjectNS'))
	die('DOM_PHP_CLASS_PI_DISABLED');
?>
--FILE--
<?php
class Foo
{

}

$Doc = new DOMDocument('1.0', 'UTF-8');
$uri = 'http://php.net/test';
$Foo = $Doc->createObjectNS($uri, 'test:root', 'Foo');
var_dump($Foo);
?>
--EXPECTF--
Warning: DOMDocument::createObjectNS() expects parameter 3 to be a class name derived from DOMElement, 'Foo' given in %s on line %d
NULL
