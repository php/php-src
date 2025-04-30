--TEST--
Entity references with stale entity declaration 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML(<<<XML
<!DOCTYPE foo [
<!ENTITY foo1 "bar1">
<!ENTITY foo2 "bar2">
<!ENTITY foo3 "bar3">
]>
<foo>&foo1;</foo>
XML);

$ref = $dom->documentElement->firstChild;
$decl = $ref->firstChild;

$nodes = $ref->childNodes;
$iter = $nodes->getIterator();
$iter->next();
$dom->removeChild($dom->doctype);
unset($decl);

try {
    var_dump($iter->current()->publicId);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
NULL
