--TEST--
SimpleXML: Simple document
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

var_dump(simplexml_load_file(dirname(__FILE__).'/sxe.xml'));

?>
===DONE===
--EXPECTF--
object(SimpleXMLElement)#%d (2) {
  [u"@attributes"]=>
  array(1) {
    [u"id"]=>
    unicode(5) "elem1"
  }
  [u"elem1"]=>
  object(SimpleXMLElement)#%d (3) {
    [u"@attributes"]=>
    array(1) {
      [u"attr1"]=>
      unicode(5) "first"
    }
    [u"comment"]=>
    object(SimpleXMLElement)#%d (0) {
    }
    [u"elem2"]=>
    object(SimpleXMLElement)#%d (1) {
      [u"elem3"]=>
      object(SimpleXMLElement)#%d (1) {
        [u"elem4"]=>
        object(SimpleXMLElement)#%d (1) {
          [u"test"]=>
          object(SimpleXMLElement)#%d (0) {
          }
        }
      }
    }
  }
}
===DONE===
