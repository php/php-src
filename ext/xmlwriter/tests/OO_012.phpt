--TEST--
XMLWriter: libxml2 XML Writer, reflection returns correct name.
--SKIPIF--
<?php
if (!extension_loaded("xmlwriter")) die("skip");
if (LIBXML_VERSION < 20701) die("skip: libxml2 2.7.1+ required");
?>
--FILE--
<?php
/* $Id$ */
$reflection = new ReflectionClass(\XMLWriter::class);
echo $reflection->getMethod('startElementNS')->getName() . "\n";
echo $reflection->getMethod('writeElementNS')->getName() . "\n";
echo $reflection->getMethod('startAttributeNS')->getName() . "\n";
echo $reflection->getMethod('writeAttributeNS')->getName() . "\n";
?>
--EXPECT--
startElementNS
writeElementNS
startAttributeNS
writeAttributeNS