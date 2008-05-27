--TEST--
SimpleXML: Simple Inheritance
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

class simplexml_inherited extends SimpleXMLElement
{
}

$xml =b<<<EOF
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

var_dump(simplexml_load_string($xml, 'simplexml_inherited'));

?>
===DONE===
--EXPECTF--
object(simplexml_inherited)#%d (2) {
  [u"@attributes"]=>
  array(1) {
    [u"id"]=>
    unicode(5) "elem1"
  }
  [u"elem1"]=>
  object(simplexml_inherited)#%d (3) {
    [u"@attributes"]=>
    array(1) {
      [u"attr1"]=>
      unicode(5) "first"
    }
    [u"comment"]=>
    object(simplexml_inherited)#%d (0) {
    }
    [u"elem2"]=>
    object(simplexml_inherited)#%d (1) {
      [u"elem3"]=>
      object(simplexml_inherited)#%d (1) {
        [u"elem4"]=>
        object(simplexml_inherited)#%d (1) {
          [u"test"]=>
          object(simplexml_inherited)#%d (0) {
          }
        }
      }
    }
  }
}
===DONE===
