--TEST--
Bug #41287 (Writing empty tags with Xmlwriter::WriteElement[ns])
--FILE--
<?php
$xml = new XmlWriter();
$xml->openMemory();
$xml->setIndent(true);
$xml->startDocument();
$xml->startElement('test');
$xml->writeElement('foo', null);
$xml->writeElement('foo2', null);
$xml->startElement('bar');
$xml->endElement('bar');
$xml->endElement();
print $xml->flush(true);

print "\n";

$xw = new XMLWriter();
$xw->openMemory();
$xw->setIndent(true);
$xw->startDocument();
$xw->startElementNS('test', 'test', 'urn:x-test:');
$xw->writeElementNS('test', 'foo', null, '');
$xw->writeElementNS(null, 'bar', 'urn:x-test:', '');
$xw->writeElementNS(null, 'bar', '', '');
$xw->endElement();
$xw->endDocument();
print $xw->flush(true);
?>
--EXPECTF--
<?xml version="1.0"?>
<test>
 <foo/>
 <foo2/>
 <bar/>
</test>

<?xml version="1.0"?>
<test:test xmlns:test="urn:x-test:">
 <test:foo/>
 <bar xmlns="urn:x-test:"/>
 <bar xmlns=""/>
</test:test>
