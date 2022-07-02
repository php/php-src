--TEST--
SimpleXML: addChild and addAttribute
--EXTENSIONS--
simplexml
--FILE--
<?php
$xml =<<<EOF
<root s:att1="b" att1="a"
      xmlns:s="urn::test" xmlns:t="urn::test-t">
   <child1>test</child1>
   <child1>test 2</child1>
   <s:child3 />
</root>
EOF;

$sxe = simplexml_load_string($xml);

/* Add new attribute in a new namespace */
$sxe->addAttribute('v:att11', 'xxx', 'urn::test-v');

/* Try to add attribute again -> display warning as method is for new Attr only */
$sxe->addAttribute('v:att11', 'xxx', 'urn::test-v');

/* Add new attribute w/o namespace */
$sxe->addAttribute('att2', 'no-ns');

$d = $sxe->attributes();
/* Try to add element to attribute -> display warning and do not add */
$d->addChild('m:test', 'myval', 'urn::test');


/* Test adding elements in various configurations */
$sxe->addChild('m:test1', 'myval', 'urn::test');

/* New namespace test */
$n = $sxe->addChild('m:test2', 'myval', 'urn::testnew');

$sxe->addChild('test3', 'myval', 'urn::testnew');
$sxe->addChild('test4', 'myval');

/* Does not add prefix here although name is valid (but discouraged) - change behavior? */
$sxe->addChild('s:test5', 'myval');

echo $sxe->asXML();
?>
--EXPECTF--
Warning: SimpleXMLElement::addAttribute(): Attribute already exists in %s031.php on line %d

Warning: SimpleXMLElement::addChild(): Cannot add element to attributes in %s031.php on line %d
<?xml version="1.0"?>
<root xmlns:s="urn::test" xmlns:t="urn::test-t" xmlns:v="urn::test-v" s:att1="b" att1="a" v:att11="xxx" att2="no-ns">
   <child1>test</child1>
   <child1>test 2</child1>
   <s:child3/>
<s:test1>myval</s:test1><m:test2 xmlns:m="urn::testnew">myval</m:test2><test3 xmlns="urn::testnew">myval</test3><test4>myval</test4><test5>myval</test5></root>
