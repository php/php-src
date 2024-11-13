--TEST--
GH-14638: null pointer dereference on object cast __toString after failed XML parsing
--EXTENSIONS--
simplexml
--CREDITS--
YuanchengJiang
--FILE--
<?php
$xml = '<?xml version="1.0" encoding="utf-8" ?>
<test>
</test>';
$root = simplexml_load_string($xml);
try {
    $root->__construct("malformed");
} catch (Exception $e) {
    // Intentionally empty
}
echo $root;
?>
--EXPECTF--
Warning: SimpleXMLElement::__construct(): Entity: line 1: parser error : Start tag expected, '<' not found in %s on line %d

Warning: SimpleXMLElement::__construct(): malformed in %s on line %d

Warning: SimpleXMLElement::__construct(): ^ in %s on line %d
