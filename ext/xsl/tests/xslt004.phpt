--TEST--
Test 4: Checking UTF8 Output
--SKIPIF--
<?php require_once __DIR__ .'/skipif.inc'; ?>
--FILE--
<?php
echo "Test 4: Checking UTF8 Output";
include("prepare.inc");
$xp = new domxpath($xsl);
$res = $xp->query("/xsl:stylesheet/xsl:output/@encoding");
if ($res->length != 1) {
    print "No or more than one xsl:output/@encoding found";
    exit;
}
$res->item(0)->value = "utf-8";
$proc->importStylesheet($xsl);
print "\n";
print $proc->transformToXml($dom);
print "\n";
--EXPECT--
Test 4: Checking UTF8 Output
<?xml version="1.0" encoding="utf-8"?>
<html><body>bar
a1 b1 c1 <br/> 
a2 c2 <br/> 
ä3 b3 c3 <br/> 
</body></html>
