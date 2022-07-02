--TEST--
XMLWriter: libxml2 XML Writer, fullEndElement method
--CREDITS--
Mauricio Vieira <mauricio [at] @mauriciovieira [dot] net>
#testfest PHPSP on 2014-07-05
--EXTENSIONS--
xmlwriter
--FILE--
<?php

$xw = new XMLWriter();
$xw->openMemory();
$xw->setIndent(TRUE);
$xw->setIndentString('   ');
$xw->startDocument('1.0', "UTF-8");
$xw->startElement('root');
$xw->startElementNS('ns1', 'child1', 'urn:ns1');
$xw->writeAttributeNS('ns1', 'att1', 'urn:ns1', '<>"\'&');
$xw->writeElement('chars', "special characters: <>\"'&");
$xw->endElement();
$xw->startElement('empty');
$xw->fullEndElement();
$xw->fullEndElement();
// Force to write and empty the buffer
$output = $xw->flush(true);
print $output;
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root>
   <ns1:child1 ns1:att1="&lt;&gt;&quot;'&amp;" xmlns:ns1="urn:ns1">
      <chars>special characters: &lt;&gt;&quot;'&amp;</chars>
   </ns1:child1>
   <empty></empty>
</root>
