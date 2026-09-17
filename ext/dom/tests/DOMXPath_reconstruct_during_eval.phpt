--TEST--
DOMXPath::__construct() during expression evaluation throws an error
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root><a>1</a><b>2</b></root>');
$xpath = new DOMXPath($doc);
$xpath->registerNamespace('php', 'http://php.net/xpath');
$xpath->registerPhpFunctions();

$other = new DOMDocument();
$other->loadXML('<other/>');

function reinit() {
    $GLOBALS['xpath']->__construct($GLOBALS['other']);
    return 'z';
}

try {
    var_dump($xpath->evaluate('string(php:function("reinit"))'));
} catch (Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

/* The object must still be usable for evaluation afterwards */
var_dump($xpath->evaluate('string(/root/a)'));
?>
--EXPECT--
Error: Cannot re-initialize DOMXPath while an XPath expression is being evaluated
string(1) "1"
