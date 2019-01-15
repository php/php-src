--TEST--
DOMDocument::createObject() <?php-class ...?> PI may not be moved
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
class Foo extends DOMElement
{

}

$Doc = new DOMDocument('1.0', 'UTF-8');
$Root = $Doc->appendChild($Doc->createElement('root'));
$Foo = $Root->appendChild($Doc->createObject('foo', 'Foo'));
$Bar = $Root->appendChild($Doc->createElement('bar'));

try {
	$Bar->appendChild($Foo->firstChild);
} catch (DOMException $Ex) {
	echo $Ex->getMessage() . "\n";
	if ($Ex->getCode() == DOM_PHP_ERR) {
		echo "OK\n";
	}
}
?>
--EXPECT--
<?php-class ...?> processing instruction may not be moved
OK
