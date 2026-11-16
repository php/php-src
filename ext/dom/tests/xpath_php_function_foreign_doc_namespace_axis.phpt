--TEST--
DOMXPath: namespace axis result on a node from a document created inside an XPath callback
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;
$doc->loadXML('<root/>');

$xp = new DOMXPath($doc);
$xp->registerNamespace('my', 'my.ns');
$xp->registerPHPFunctionNS('my.ns', 'include', function () {
    $d = new DOMDocument;
    $d->loadXML('<r><uaf xmlns:a="urn:a"/></r>');
    return $d->documentElement;
});

$result = $xp->query('my:include()/uaf/namespace::*');
var_dump($result->length);
unset($xp);

foreach ($result as $ns) {
    var_dump($ns->nodeName, $ns->parentNode->nodeName);
}

echo "Done\n";

?>
--EXPECT--
int(2)
string(9) "xmlns:xml"
string(3) "uaf"
string(7) "xmlns:a"
string(3) "uaf"
Done
