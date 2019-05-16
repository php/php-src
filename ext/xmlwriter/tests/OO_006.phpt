--TEST--
XMLWriter: libxml2 XML Writer, startDTD/writeElementNS
--SKIPIF--
<?php
if (!extension_loaded("xmlwriter")) die("skip");
?>
--FILE--
<?php

$doc_dest = 'OO_006.xml';
$xw = new XMLWriter();
$xw->openUri($doc_dest);
$xw->startDtd('foo', NULL, 'urn:bar');
$xw->endDtd();
$xw->startElement('foo');
$xw->writeElementNS('foo', 'bar', 'urn:foo', 'dummy content');
$xw->endElement();

// Force to write and empty the buffer
$output_bytes = $xw->flush(true);
echo file_get_contents($doc_dest);
unset($xw);
unlink($doc_dest);
?>
--EXPECT--
<!DOCTYPE foo SYSTEM "urn:bar"><foo><foo:bar xmlns:foo="urn:foo">dummy content</foo:bar></foo>
