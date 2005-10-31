--TEST--
SPL: SimpleXMLIterator and getChildren()
--SKIPIF--
<?php 
if (!extension_loaded("spl")) print "skip";
if (!extension_loaded('simplexml')) print 'skip';
if (!extension_loaded("libxml")) print "skip LibXML not present";
if (!class_exists('RecursiveIteratorIterator')) print 'skip RecursiveIteratorIterator not available';
?>
--FILE--
<?php 

$xml =<<<EOF
<?xml version='1.0'?>
<sxe>
 <elem1/>
 <elem2/>
 <elem2/>
</sxe>
EOF;

class SXETest extends SimpleXMLIterator
{
	function count()
	{
		echo __METHOD__ . "\n";
		return parent::count();
	}
}

$sxe = new SXETest($xml);

var_dump(count($sxe));
var_dump(count($sxe->elem1));
var_dump(count($sxe->elem2));

?>
===DONE===
--EXPECT--
SXETest::count
int(3)
SXETest::count
int(1)
SXETest::count
int(2)
===DONE===
