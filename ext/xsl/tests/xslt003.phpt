--TEST--
Test 1: Using Parameters
--FILE--
<?php
echo "Test 1: Using Parameters";
include("prepare.inc");
$proc->importStylesheet($xsl);
$proc->setParameter( "", "foo","hello world");
print "\n";
print $proc->transformToXml($dom);
print "\n";


--EXPECT--
Test 1: Transform To XML String
<?xml version="1.0" encoding="iso-8859-1"?>
<html><body>bar
a1 b1 c1 <br/> 
a2 c2 <br/> 
a3 b3 c3 <br/> 
</body></html>
