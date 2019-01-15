--TEST--
DOMDocument::loadXML() with <?php-class ...?> PIs
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

class Bar
{

}

$xml = '<?xml version="1.0" encoding="UTF-8"?>
<root>
  <object><?php-class Foo?></object>
  <object><?php-class Bar?></object>
  <object><?php-class Chi?></object>
</root>';

$Doc = new DOMDocument();
$Doc->preserveWhiteSpace = false;
$Doc->loadXML($xml);

echo get_class($Doc->documentElement) . "\n";
echo get_class($Doc->documentElement->firstChild) . "\n";
echo get_class($Doc->documentElement->firstChild->nextSibling) . "\n";
echo get_class($Doc->documentElement->lastChild) . "\n";
?>
--EXPECTF--
DOMElement
Foo

Warning: %s: Class Bar is not derived from DOMElement; ignoring <?php-class Bar?> in %s on line %d
DOMElement

Warning: %s: Class Chi does not exist; ignoring <?php-class Chi?> in %s on line %d
DOMElement
