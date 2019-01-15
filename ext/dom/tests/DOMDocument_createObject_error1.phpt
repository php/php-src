--TEST--
DOMDocument::createObject() class must extend DOMElement
--CREDITS--
Adam Martinson
--SKIPIF--
<?php
require_once('skipif.inc');
if (!method_exists('DOMDocument', 'createObject'))
	die('DOM_PHP_CLASS_PI_DISABLED');
?>
--FILE--
<?php
class Foo
{

}

$Doc = new DOMDocument('1.0', 'UTF-8');
$Foo = $Doc->createObject('root', 'Foo');
var_dump($Foo);
?>
--EXPECTF--
Warning: DOMDocument::createObject() expects parameter 2 to be a class name derived from DOMElement, 'Foo' given in %s on line %d
NULL
