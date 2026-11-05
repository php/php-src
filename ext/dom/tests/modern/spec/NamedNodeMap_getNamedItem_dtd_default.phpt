--TEST--
getNamedItem() must not expose DTD attribute declarations
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<<XML
<?xml version="1.0"?>
<!DOCTYPE root [
<!ELEMENT root EMPTY>
<!ATTLIST root defaulted CDATA "from-dtd">
]>
<root real="present"/>
XML;

$legacy = new DOMDocument();
$legacy->loadXML($xml);

foreach (['legacy' => $legacy, 'spec' => Dom\XMLDocument::createFromString($xml)] as $label => $doc) {
    echo $label, PHP_EOL;
    $map = $doc->documentElement->attributes;
    foreach (['defaulted', 'real'] as $name) {
        var_dump($map->getNamedItem($name)?->nodeValue);
        var_dump(isset($map[$name]));
        var_dump($map[$name]?->nodeValue);
    }
}
?>
--EXPECT--
legacy
NULL
bool(false)
NULL
string(7) "present"
bool(true)
string(7) "present"
spec
NULL
bool(false)
NULL
string(7) "present"
bool(true)
string(7) "present"
