--TEST--
Attributes: DOMAttribute functionality
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML($xmlstr);
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$node = $dom->documentElement;

$lang = $node->getAttributeNode('language');
echo "Language: ".$lang->value."\n";

$lang->value = 'en-US';
echo "Language: ".$lang->value."\n";

$parent = $lang->ownerElement;

$chapter = new DOMAttr("num", "1");
$parent->setAttributeNode($chapter);

echo "Is ID?: ".($chapter->isId()?'YES':'NO')."\n";

$top_element = $node->cloneNode();

print $dom->saveXML($top_element);


?>
--EXPECT--

Language: en
Language: en-US
Is ID?: NO
<chapter language="en-US" num="1"/>

