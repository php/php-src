--TEST--
getNamedItemNS() must match on the local name in a namespace, not on the qualified name
--EXTENSIONS--
dom
--FILE--
<?php
$src = '<root xmlns:q="urn:q" bar="no-ns" q:bar="ns" q:only="prefixed"/>';
$dtd = <<<XML
<?xml version="1.0"?>
<!DOCTYPE root [
<!ELEMENT root EMPTY>
<!ATTLIST root defaulted CDATA "from-dtd">
]>
<root/>
XML;

$legacy = new DOMDocument();
$legacy->loadXML($src);
$legacyDtd = new DOMDocument();
$legacyDtd->loadXML($dtd);

foreach (['legacy' => $legacy, 'spec' => Dom\XMLDocument::createFromString($src)] as $label => $doc) {
    echo $label, PHP_EOL;
    $map = $doc->documentElement->attributes;
    var_dump($map->getNamedItemNS(null, 'bar')?->nodeValue);
    var_dump($map->getNamedItemNS('', 'bar')?->nodeValue);
    var_dump($map->getNamedItemNS(null, 'q:bar')?->nodeValue);
    var_dump($map->getNamedItemNS('', 'q:bar')?->nodeValue);
    var_dump($map->getNamedItemNS(null, 'only')?->nodeValue);
    var_dump($map->getNamedItemNS('urn:q', 'only')?->nodeValue);
    var_dump($map->getNamedItem('q:bar')?->nodeValue);
}

echo 'dtd default', PHP_EOL;
var_dump($legacyDtd->documentElement->attributes->getNamedItemNS(null, 'defaulted')?->nodeValue);
var_dump(Dom\XMLDocument::createFromString($dtd)->documentElement->attributes->getNamedItemNS(null, 'defaulted')?->nodeValue);
?>
--EXPECT--
legacy
string(5) "no-ns"
NULL
NULL
NULL
NULL
string(8) "prefixed"
NULL
spec
string(5) "no-ns"
string(5) "no-ns"
NULL
NULL
NULL
string(8) "prefixed"
string(2) "ns"
dtd default
NULL
NULL
