--TEST--
GH-22554 (Use-after-free when an XPath callback returns a node from a document created inside the callback)
--CREDITS--
waseem-cve
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
    $d->loadXML('<r><uaf/></r>');

    return $d->documentElement;
});

$xp->registerPHPFunctionNS('my.ns', 'process', function ($arg) {
    echo "process arg: ", get_class($arg[0]), " ", $arg[0]->nodeName, "\n";
    return 'x';
});

$result = $xp->query('my:process(my:include()/uaf)');
var_dump($result->length);
unset($xp);

echo "Done\n";

?>
--EXPECT--
process arg: DOMElement uaf
int(0)
Done
