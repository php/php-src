--TEST--
SimpleXML and attributes
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_file(dirname(__FILE__).'/sxe.xml');

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
===Done===
--EXPECTF--
===Property===
object(simplexml_element)#%d (2) {
  ["comment"]=>
  object(simplexml_element)#%d (0) {
  }
  ["elem2"]=>
  object(simplexml_element)#%d (1) {
    ["elem3"]=>
    object(simplexml_element)#%d (1) {
      ["elem4"]=>
      object(simplexml_element)#%d (1) {
        ["test"]=>
        object(simplexml_element)#%d (0) {
        }
      }
    }
  }
}
===Array===
string(5) "elem1"
string(5) "first"
===Set===
string(8) "Changed1"
string(2) "12"
===Unset===
NULL
NULL
===Misc.===
int(4)
int(4)
===Done===
