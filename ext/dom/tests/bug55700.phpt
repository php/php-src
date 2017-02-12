--TEST--
Bug #55700 (Disable automatic registration on a DOMXpath object)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(
    '<foobar><a:foo xmlns:a="urn:a">'.
    '<b:bar xmlns:b="urn:b"/></a:foo>'.
    '</foobar>'
);
$xpath = new DOMXPath($dom);

// disable automatic namespace registration
var_dump($xpath->enableRegisterNodeNS);
$xpath->enableRegisterNodeNS = FALSE;
var_dump($xpath->enableRegisterNodeNS);

$context = $dom->documentElement->firstChild;
$xpath->registerNamespace('a', 'urn:b');
var_dump(
    $xpath->evaluate(
        'descendant-or-self::a:*',
        $context
    )->item(0)->tagName
);

?>
--EXPECT--
bool(true)
bool(false)
string(5) "b:bar"
