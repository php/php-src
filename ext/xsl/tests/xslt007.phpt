--TEST--
Test 7: Transform To Uri
--EXTENSIONS--
xsl
--FILE--
<?php
echo "Test 7: Transform To Uri";
include("prepare.inc");
$proc->importStylesheet($xsl);
print "\n";
$doc = $proc->transformToUri($dom, "file://".__DIR__."/out.xml");
print file_get_contents(__DIR__."/out.xml");
unlink(__DIR__."/out.xml");
print "\n";
?>
--EXPECT--
Test 7: Transform To Uri
<?xml version="1.0" encoding="iso-8859-1"?>
<html><body>bar
a1 b1 c1 <br/> 
a2 c2 <br/> 
ä3 b3 c3 <br/> 
</body></html>
