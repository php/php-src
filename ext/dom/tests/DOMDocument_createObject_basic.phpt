--TEST--
DOMDocument::createObject()
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

class Bar extends DOMElement
{

}

class Chi extends DOMElement
{

}

$Doc = new DOMDocument('1.0', 'UTF-8');
$Doc->preserveWhiteSpace = false;
$Doc->formatOutput = true;

$Root = $Doc->createElement('root');
$Doc->appendChild($Root);

$Foo = $Doc->createObject('empty', 'Foo');
$Root->appendChild($Foo);

$Bar = $Doc->createObject('complex', 'Bar');
$Root->appendChild($Bar);

$Chi = $Doc->createObject('simple', 'Chi', 'chi');
$Bar->appendChild($Chi);

$Doc->normalizeDocument();

echo get_class($Root) . "\n";
echo get_class($Foo) . "\n";
echo $Foo->textContent . "\n";
echo get_class($Bar) . "\n";
echo $Bar->textContent . "\n";
echo get_class($Chi) . "\n";
echo $Chi->textContent . "\n";

echo "\n";
unset($Foo);
unset($Bar);
unset($Chi);
unset($Root);

echo get_class($Doc->documentElement->firstChild) . "\n";
echo get_class($Doc->documentElement->lastChild) . "\n";
echo get_class($Doc->documentElement->lastChild->lastChild) . "\n";

$xml = $Doc->saveXML();
unset($Doc);
echo "\n$xml\n";

$Doc = new DOMDocument();
$Doc->preserveWhiteSpace = false;
$Doc->loadXML($xml);
echo get_class($Doc->documentElement) . "\n";
echo get_class($Doc->documentElement->firstChild) . "\n";
echo get_class($Doc->documentElement->lastChild) . "\n";
echo get_class($Doc->documentElement->lastChild->lastChild) . "\n";
?>
--EXPECT--
DOMElement
Foo

Bar
chi
Chi
chi

Foo
Bar
Chi

<?xml version="1.0" encoding="UTF-8"?>
<root>
  <empty>
    <?php-class Foo?>
  </empty>
  <complex>
    <?php-class Bar?>
    <simple><?php-class Chi?>chi</simple>
  </complex>
</root>

DOMElement
Foo
Bar
Chi
