--TEST--
Test DOMDocument::createAttribute() for expected return value
--FILE--
<?php
$dom = new DOMDocument();

$attr = $dom->createAttribute('string');
echo get_class($attr);

?>
--EXPECTF--
DOMAttr
