--TEST--
XMLWriter: libxml2 XML Writer, Write CDATA
--CREDITS--
Mark Baker mark@lange.demon.co.uk at the PHPNW2017 Conference for PHP Testfest 2017
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php

$cDataString = "Text for inclusion within CData tags can include characters like <, >, &, and quotes like ' and \"";
$xmlWriter = new XmlWriter();
$xmlWriter->openMemory();

$xmlWriter->startDocument('1.0', 'UTF-8');
$xmlWriter->startElement('myDocumentRoot');
$xmlWriter->startElement('myElement');
// CData output
$xmlWriter->startElement('cdataElement');
$xmlWriter->writeCData($cDataString);
$xmlWriter->endElement();
// end the document and output
$xmlWriter->endElement();
$xmlWriter->endElement();

echo $xmlWriter->outputMemory(true);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<myDocumentRoot><myElement><cdataElement><![CDATA[Text for inclusion within CData tags can include characters like <, >, &, and quotes like ' and "]]></cdataElement></myElement></myDocumentRoot>
