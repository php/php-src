--TEST--
Test 9: Stream Wrapper XPath-Document()
--EXTENSIONS--
zlib
xsl
--FILE--
<?php
echo "Test 9: Stream Wrapper XPath-Document()";
include("prepare.inc");

$xsl = new domDocument;
$xsl->load(__DIR__."/documentxpath.xsl");
if(!$xsl) {
  echo "Error while parsing the document\n";
  exit;
}

$proc->importStylesheet($xsl);
print "\n";
print $proc->transformToXML($dom);
?>
--EXPECT--
Test 9: Stream Wrapper XPath-Document()
<?xml version="1.0" encoding="iso-8859-1"?>
foo
