--TEST--
SimpleXML: Attributes inside foreach
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$xml =b<<<EOF
<?xml version='1.0'?>
<pres><content><file glob="slide_*.xml"/></content></pres>
EOF;

$sxe = simplexml_load_string($xml);

echo "===CONTENT===\n";
var_dump($sxe->content);

echo "===FILE===\n";
var_dump($sxe->content->file);

echo "===FOREACH===\n";
foreach($sxe->content->file as $file)
{
	var_dump($file);
	var_dump($file['glob']);
}

?>
===DONE===
--EXPECTF--
===CONTENT===
object(SimpleXMLElement)#%d (1) {
  [u"file"]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"glob"]=>
      unicode(11) "slide_*.xml"
    }
  }
}
===FILE===
object(SimpleXMLElement)#%d (1) {
  [u"@attributes"]=>
  array(1) {
    [u"glob"]=>
    unicode(11) "slide_*.xml"
  }
}
===FOREACH===
object(SimpleXMLElement)#%d (1) {
  [u"@attributes"]=>
  array(1) {
    [u"glob"]=>
    unicode(11) "slide_*.xml"
  }
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  unicode(11) "slide_*.xml"
}
===DONE===
