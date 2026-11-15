--TEST--
Anonymous class names are not truncated in DOM error messages
--EXTENSIONS--
dom
--FILE--
<?php

function reconstruct() {
    try {
        $GLOBALS['xpath']->__construct($GLOBALS['doc']);
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

$doc = new DOMDocument();
$doc->loadXML('<root/>');

$xpath = new class($doc) extends DOMXPath {};
$xpath->registerNamespace('php', 'http://php.net/xpath');
$xpath->registerPhpFunctions();
$xpath->evaluate('string(php:function("reconstruct"))');

?>
--EXPECTF--
Error: Cannot call DOMXPath@anonymous%0%s::__construct() while an XPath evaluation is in progress
