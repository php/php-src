--TEST--
SPL: SimpleXMLIterator
--SKIPIF--
<?php
if (!extension_loaded("simplexml")) print "skip SimpleXML not present";
if (!extension_loaded("libxml")) print "skip LibXML not present";
?>
--FILE--
<?php 

$xml =<<<EOF
<?xml version='1.0'?>
<!DOCTYPE sxe SYSTEM "notfound.dtd">
<sxe id="elem1">
 <elem1 attr1='first'>
  <!-- comment -->
  <elem2>
   <elem3>
    <elem4>
     <?test processing instruction ?>
    </elem4>
   </elem3>
  </elem2>
 </elem1>
</sxe>
EOF;

var_dump(simplexml_load_string((binary)$xml, 'SimpleXMLIterator'));

?>
===DONE===
--EXPECTF--
object(SimpleXMLIterator)#%d (2) {
  [u"@attributes"]=>
  array(1) {
    [u"id"]=>
    unicode(5) "elem1"
  }
  [u"elem1"]=>
  object(SimpleXMLIterator)#%d (3) {
    [u"@attributes"]=>
    array(1) {
      [u"attr1"]=>
      unicode(5) "first"
    }
    [u"comment"]=>
    object(SimpleXMLIterator)#%d (0) {
    }
    [u"elem2"]=>
    object(SimpleXMLIterator)#%d (1) {
      [u"elem3"]=>
      object(SimpleXMLIterator)#%d (1) {
        [u"elem4"]=>
        object(SimpleXMLIterator)#%d (1) {
          [u"test"]=>
          object(SimpleXMLIterator)#%d (0) {
          }
        }
      }
    }
  }
}
===DONE===
