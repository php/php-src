--TEST--
SimpleXMLElement::__toString() OOM
--EXTENSIONS--
simplexml
--INI--
memory_limit=16M
--FILE--
<?php
$xml = simplexml_load_string('<r>' . str_repeat('x', 5500000) . '</r>', 'SimpleXMLElement', LIBXML_PARSEHUGE);
$pad = str_repeat('p', 11500000);
$value = $xml->__toString();
echo strlen($value), "\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in %s on line 4
