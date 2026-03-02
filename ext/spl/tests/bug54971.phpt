--TEST--
Bug #54971 (Wrong result when using iterator_to_array with use_keys on true)
--EXTENSIONS--
dom
--FILE--
<?php

$source = <<<XML
<root>
<node>val1</node>
<node>val2</node>
</root>
XML;


$doc = new DOMDocument();
$doc->loadXML($source);

$xpath = new DOMXPath($doc);
$items = $xpath->query('//node');

print_r(array_map('get_class', iterator_to_array($items, false)));
print_r(array_map('get_class', iterator_to_array($items, true)));
?>
--EXPECT--
Array
(
    [0] => DOMElement
    [1] => DOMElement
)
Array
(
    [0] => DOMElement
    [1] => DOMElement
)
