--TEST--
Test 2: Transform To HTML String
--SKIPIF--
<?php require_once __DIR__ .'/skipif.inc'; ?>
--FILE--
<?php
echo "Test 2: Transform To HTML String";
include("prepare.inc");
// changing output method to html
$xp = new domxpath($xsl);
$res = $xp->query("/xsl:stylesheet/xsl:output/@method");
if ($res->length != 1) {
    print "No or more than one xsl:output/@method found";
    exit;
}
$res->item(0)->value = "html";
$proc->importStylesheet($xsl);
print "\n";
print $proc->transformToXml($dom);
print "\n";
--EXPECT--
Test 2: Transform To HTML String
<html><body>bar
a1 b1 c1 <br> 
a2 c2 <br> 
ä3 b3 c3 <br> 
</body></html>
