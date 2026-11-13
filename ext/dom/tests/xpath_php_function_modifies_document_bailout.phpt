--TEST--
DOMXPath: a document becomes writable again after a fatal error in a php:function() callback
--EXTENSIONS--
dom
zend_test
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('skip LeakSanitizer reports the libxml2 XPath state that the bailout abandons');
?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root><a/><b/></root>');
$xp = new DOMXPath($doc);
$xp->registerNamespace('php', 'http://php.net/xpath');
$xp->registerPhpFunctions();

function cb() {
    try {
        $GLOBALS['doc']->documentElement->removeChild($GLOBALS['doc']->documentElement->firstChild);
    } catch (DOMException $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    zend_trigger_bailout();
}

register_shutdown_function(function () use ($doc) {
    $doc->documentElement->removeChild($doc->documentElement->firstChild);
    echo $doc->saveXML($doc->documentElement), "\n";
});

$xp->evaluate('boolean(php:function("cb"))');
?>
--EXPECTF--
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress

Fatal error: Bailout in %s on line %d
<root><b/></root>
