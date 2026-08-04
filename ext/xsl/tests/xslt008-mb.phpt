--TEST--
Test 8: Stream Wrapper Includes
--EXTENSIONS--
zlib
xsl
--FILE--
<?php
echo "Test 8: Stream Wrapper Includes ";
include("prepare.inc");
$xsl = new DOMDocument;
$xsl->load(__DIR__."/私はガラスを食べられますstreamsinclude.xsl");
if(!$xsl) {
  echo "Error while parsing the document\n";
  exit;
}
chdir(__DIR__);
$proc->importStylesheet($xsl);
print "\n";
print $proc->transformToXml($dom);
?>
--EXPECT--
Test 8: Stream Wrapper Includes 
<?xml version="1.0" encoding="iso-8859-1"?>
<html><body>bar
a1 b1 c1 <br/> 
a2 c2 <br/> 
�3 b3 c3 <br/> 
</body></html>
