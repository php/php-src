--TEST--
SimpleXML: Entities
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$xml =b<<<EOF
<?xml version='1.0'?>
<!DOCTYPE sxe SYSTEM "notfound.dtd" [
<!ENTITY included-entity "This is text included from an entity">
]>
<sxe id="elem1">
 Plain text.
 <elem1 attr1='first'>
  <!-- comment -->
  <elem2>
   <elem3>
    &included-entity;
    <elem4>
     <?test processing instruction ?>
    </elem4>
   </elem3>
  </elem2>
 </elem1>
</sxe>
EOF;

var_dump(simplexml_load_string($xml));

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
      object(SimpleXMLElement)#%d (2) {
        [u"included-entity"]=>
        object(SimpleXMLElement)#%d (1) {
          [u"included-entity"]=>
          unicode(36) "This is text included from an entity"
        }
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
