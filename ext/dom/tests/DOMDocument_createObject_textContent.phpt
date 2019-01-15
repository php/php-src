--TEST--
DOMDocument::createObject()->textContent = "chi"
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
$Foo = $Doc->createObject('root', 'Foo', "bar");
$Doc->appendChild($Foo);

echo $Foo->textContent . "\n";
$Foo->textContent = "chi";
echo $Foo->textContent . "\n";
if ($Foo->firstChild->nodeType == XML_PI_NODE) {
	echo "OK\n";
}
?>
--EXPECT--
bar
chi
OK
