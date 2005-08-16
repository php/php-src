--TEST--
SimpleXML and attributes inside foreach
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$xml =<<<EOF
<?xml version='1.0'?>
<pres><content><file glob="slide_*.xml"/></content></pres>
EOF;

$sxe = simplexml_load_string($xml);

foreach($sxe->content->file as $file)
{
	var_dump($file);
	var_dump($file['glob']);
}

?>
===DONE===
--EXPECTF--
object(SimpleXMLElement)#%d (0) {
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(11) "slide_*.xml"
}
===DONE===
--UEXPECTF--
object(SimpleXMLElement)#%d (0) {
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  unicode(11) "slide_*.xml"
}
===DONE===
