<?php
$xw = new XMLWriter();
$xw->openUri('test.xml');
$xw->startDocument("1.0");
$xw->startElement("book");
$xw->text("example");
$xw->endElement();
$xw->endDocument();
$xw->flush(0);
