--TEST--
Test DOMDocument::createAttribute() for expected return value
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument();

$attr = $dom->createAttribute('string');
echo get_class($attr);

?>
--EXPECT--
DOMAttr
