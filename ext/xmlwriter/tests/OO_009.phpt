--TEST--
XMLWriter: PI, Comment, CDATA
--SKIPIF--
<?php
if (!extension_loaded("xmlwriter")) die("skip");
?>
--FILE--
<?php
/* $Id$ */
/*
Libxml 2.6.24 and up adds a new line after a processing instruction (PI)
*/
$xw = new XMLWriter();
$xw->openMemory();
$xw->setIndent(TRUE);
$xw->startDocument("1.0", "UTF-8");
$xw->startElement('root');
$xw->writeAttribute('id', 'elem1');
$xw->startElement('elem1');
$xw->writeAttribute('attr1', 'first');
$xw->writeComment('start PI');
$xw->startElement('pi');
$xw->writePi('php', 'echo "hello world"; ');
$xw->endElement();
$xw->startElement('cdata');
$xw->startCdata();
$xw->text('<>&"');
$xw->endCdata();
$xw->endElement();
$xw->endElement();
$xw->endElement();
$xw->endDocument();
// Force to write and empty the buffer
$output = $xw->flush(true);
print $output;
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<root id="elem1">
 <elem1 attr1="first">
  <!--start PI-->
  <pi><?php echo "hello world"; ?>%w</pi>
  <cdata><![CDATA[<>&"]]></cdata>
 </elem1>
</root>
