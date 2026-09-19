--TEST--
GH-23729 (Use-after-free when DOMXPath is reconstructed during an evaluation)
--CREDITS--
djarfluka
--EXTENSIONS--
dom
--FILE--
<?php

function reconstruct() {
    try {
        $GLOBALS['xpath']->__construct($GLOBALS['other']);
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    return 'r';
}

function nested() {
    echo 'nested: ', $GLOBALS['xpath']->evaluate('string(/root/b)'), PHP_EOL;
    return 'n';
}

function test(string $class, object $doc, object $other) {
    $xpath = new $class($doc);
    $xpath->registerNamespace('php', 'http://php.net/xpath');
    $xpath->registerPhpFunctions();

    $GLOBALS['xpath'] = $xpath;
    $GLOBALS['other'] = $other;

    var_dump($xpath->evaluate('string(php:function("reconstruct"))'));
    /* The evaluation the callback tried to destroy must still be usable. */
    var_dump($xpath->evaluate('string(/root/a)'));
    /* A nested evaluation must not lift the guard of the outer one. */
    var_dump($xpath->evaluate('concat(php:function("nested"), php:function("reconstruct"))'));
    var_dump($xpath->query('//b[php:function("reconstruct")]')->length);

    /* Reconstructing outside of an evaluation is still allowed. */
    $xpath->__construct($other);
    var_dump($xpath->document->documentElement->nodeName);
}

$doc = new DOMDocument();
$doc->loadXML('<root><a>1</a><b>2</b></root>');
$other = new DOMDocument();
$other->loadXML('<other/>');
test(DOMXPath::class, $doc, $other);

$doc = Dom\XMLDocument::createFromString('<root><a>1</a><b>2</b></root>');
$other = Dom\XMLDocument::createFromString('<other/>');
test(Dom\XPath::class, $doc, $other);

?>
--EXPECT--
Error: Cannot call DOMXPath::__construct() while an XPath evaluation is in progress
string(1) "r"
string(1) "1"
nested: 2
Error: Cannot call DOMXPath::__construct() while an XPath evaluation is in progress
string(2) "nr"
Error: Cannot call DOMXPath::__construct() while an XPath evaluation is in progress
int(1)
string(5) "other"
Error: Cannot call Dom\XPath::__construct() while an XPath evaluation is in progress
string(1) "r"
string(1) "1"
nested: 2
Error: Cannot call Dom\XPath::__construct() while an XPath evaluation is in progress
string(2) "nr"
Error: Cannot call Dom\XPath::__construct() while an XPath evaluation is in progress
int(1)
string(5) "other"
