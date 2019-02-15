--TEST--
Test 5: Checking Indent
--SKIPIF--
<?php require_once dirname(__FILE__) .'/skipif.inc'; ?>
--FILE--
<?php
echo "Test 5: Checking Indent";
include("prepare.inc");
$xp = new domxpath($xsl);
$res = $xp->query("/xsl:stylesheet/xsl:output/@indent");
if ($res->length != 1) {
    print "No or more than one xsl:output/@indent found";
    exit;
}
$res->item(0)->value = "yes";
$proc->importStylesheet($xsl);
print "\n";
print $proc->transformToXml($dom);
print "\n";
--EXPECT--
Test 5: Checking Indent
<?xml version="1.0" encoding="iso-8859-1"?>
<html>
  <body>bar
a1 b1 c1 <br/> 
a2 c2 <br/> 
ä3 b3 c3 <br/> 
</body>
</html>
