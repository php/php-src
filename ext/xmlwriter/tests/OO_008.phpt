--TEST--
XMLWriter: libxml2 XML Writer DTD Element & Attlist
--SKIPIF--
<?php
if (!extension_loaded("xmlwriter")) die("skip");
?>
--FILE--
<?php

$xw = new XMLWriter();
$xw->openMemory();
$xw->setIndent(TRUE);
$xw->startDocument(NULL, "UTF-8");
$xw->writeDtdElement('sxe', '(elem1+, elem11, elem22*)');
$xw->writeDtdAttlist('sxe', 'id     CDATA  #implied');
$xw->startDtdElement('elem1');
$xw->text('elem2*');
$xw->endDtdElement();
$xw->startDtdAttlist('elem1');
$xw->text("attr1  CDATA  #required\n");
$xw->text('attr2  CDATA  #implied');
$xw->endDtdAttlist();
$xw->endDocument();
// Force to write and empty the buffer
$output = $xw->flush(true);
print $output;
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<!ELEMENT sxe (elem1+, elem11, elem22*)>
<!ATTLIST sxe id     CDATA  #implied>
<!ELEMENT elem1 elem2*>
<!ATTLIST elem1 attr1  CDATA  #required
attr2  CDATA  #implied>
