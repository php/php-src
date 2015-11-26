--TEST--
Bug #45251 (double free or corruption with setAttributeNode())
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXml(<<<EOF
<?xml version="1.0" encoding="utf-8" ?>
<aaa>
  <bbb foo="bar"/>
</aaa>
EOF
);

$xpath = new DOMXPath($doc);

$bbb = $xpath->query('bbb', $doc->documentElement)->item(0);

$ccc = $doc->createElement('ccc');
foreach ($bbb->attributes as $attr)
{
  $ccc->setAttributeNode($attr);
}

echo $attr->parentNode->localName;

?>
--EXPECT--
ccc
