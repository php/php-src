--TEST--
DOMDocument::createObject() <?php-class ...?> PI is read-only
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
$Foo = $Doc->createObject('root', 'Foo');
$Doc->appendChild($Foo);

try {
	$Foo->firstChild->nodeValue = 'Bar';
} catch (DOMException $Ex) {
	if ($Ex->getCode() == DOM_NO_MODIFICATION_ALLOWED_ERR) {
		echo "OK\n";
	} else {
		echo "$Ex\n";
	}
}
try {
	$Foo->firstChild->textContent = 'Bar';
} catch (DOMException $Ex) {
	if ($Ex->getCode() == DOM_NO_MODIFICATION_ALLOWED_ERR) {
		echo "OK\n";
	} else {
		echo "$Ex\n";
	}
}
try {
	$Foo->firstChild->data = 'Bar';
} catch (DOMException $Ex) {
	if ($Ex->getCode() == DOM_NO_MODIFICATION_ALLOWED_ERR) {
		echo "OK\n";
	} else {
		echo "$Ex\n";
	}
}
?>
--EXPECT--
OK
OK
OK
