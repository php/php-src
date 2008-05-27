--TEST--
SimpleXML: CDATA
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_string(b<<<EOF
<?xml version='1.0'?>
<!DOCTYPE sxe SYSTEM "notfound.dtd">
<sxe id="elem1">
 Plain text.
 <elem1 attr1='first'>
  <!-- comment -->
  <elem2>
   <![CDATA[CDATA block]]>
   <elem3>
    <elem4>
     <?test processing instruction ?>
    </elem4>
   </elem3>
  </elem2>
 </elem1>
</sxe>
EOF
);

var_dump($sxe);

$elem1 = $sxe->elem1;
$elem2 = $elem1->elem2;
var_dump(trim((string)$elem2));

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
unicode(11) "CDATA block"
===DONE===
