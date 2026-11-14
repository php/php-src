--TEST--
DOMXPath: a document stays read-only while a Fiber is suspended inside a php:function() callback
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root><a/><b/><c/></root>');
$xp = new DOMXPath($doc);
$xp->registerNamespace('php', 'http://php.net/xpath');
$xp->registerPhpFunctions();

function suspend() {
    Fiber::suspend();
    return true;
}

$fiber = new Fiber(function () use ($xp) {
    $names = [];
    foreach ($xp->query('/root/*[php:function("suspend")]') as $node) {
        $names[] = $node->nodeName;
    }
    return $names;
});
$fiber->start();

try {
    $doc->documentElement->removeChild($doc->documentElement->firstChild);
} catch (DOMException $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

while (!$fiber->isTerminated()) {
    $fiber->resume();
}
echo implode(",", $fiber->getReturn()), "\n";

$doc->documentElement->removeChild($doc->documentElement->firstChild);
echo $doc->saveXML($doc->documentElement), "\n";
?>
--EXPECT--
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
a,b,c
<root><b/><c/></root>
