--TEST--
SimpleXML: Attributes
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
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

$sxe = simplexml_load_string($xml);

echo "===Property===\n";
var_dump($sxe->elem1);
echo "===Array===\n";
var_dump($sxe['id']);
var_dump($sxe->elem1['attr1']);
echo "===Set===\n";
$sxe['id'] = "Changed1";
var_dump($sxe['id']);
$sxe->elem1['attr1'] = 12;
var_dump($sxe->elem1['attr1']);
echo "===Unset===\n";
unset($sxe['id']);
var_dump($sxe['id']);
unset($sxe->elem1['attr1']);
var_dump($sxe->elem1['attr1']);
echo "===Misc.===\n";
$a = 4;
var_dump($a);
$dummy = $sxe->elem1[$a];
var_dump($a);
?>
--EXPECTF--
===Property===
object(SimpleXMLElement)#%d (3) {
  ["@attributes"]=>
  array(1) {
    ["attr1"]=>
    string(5) "first"
  }
  ["comment"]=>
  object(SimpleXMLElement)#%d (0) {
  }
  ["elem2"]=>
  object(SimpleXMLElement)#%d (1) {
    ["elem3"]=>
    object(SimpleXMLElement)#%d (1) {
      ["elem4"]=>
      object(SimpleXMLElement)#%d (1) {
        ["test"]=>
        object(SimpleXMLElement)#%d (0) {
        }
      }
    }
  }
}
===Array===
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(5) "elem1"
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(5) "first"
}
===Set===
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(8) "Changed1"
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(2) "12"
}
===Unset===
NULL
NULL
===Misc.===
int(4)
int(4)
