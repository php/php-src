--TEST--
Test 3: Using Parameters
--EXTENSIONS--
xsl
--FILE--
<?php
echo "Test 3: Using Parameters";
include("prepare.inc");
$proc->importStylesheet($xsl);
$proc->setParameter( "", "foo","hello world");
print "\n";
print $proc->transformToXml($dom);
print "\n";
?>
--EXPECT--
Test 3: Using Parameters
<?xml version="1.0" encoding="iso-8859-1"?>
<html><body>hello world
a1 b1 c1 <br/> 
a2 c2 <br/> 
ä3 b3 c3 <br/> 
</body></html>
