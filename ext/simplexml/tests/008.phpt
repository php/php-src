--TEST--
SimpleXML and XPath 
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 
$sxe = simplexml_load_file(dirname(__FILE__).'/sxe.xml');
var_dump($sxe->xpath("elem1/elem2/elem3/elem4"));
var_dump($sxe->xpath("***"));
?>
--EXPECTF--
array(1) {
  [0]=>
  object(simplexml_element)#%d (1) {
    ["test"]=>
    object(simplexml_element)#%d (0) {
    }
  }
}
bool(false)
