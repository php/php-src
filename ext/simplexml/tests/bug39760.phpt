--TEST--
Bug #39760 (cloning fails on nested SimpleXML-Object)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = '<?xml version="1.0" ?>
<test>
    <level1>
        <level2a>text1</level2a>
        <level2b>text2</level2b>
   </level1>
</test>';
$test = simplexml_load_string($xml);

var_dump($test->level1->level2a);

$test2 = clone $test;
var_dump($test2->level1->level2a);

$test3 = clone $test->level1->level2a;
var_dump($test3);

echo "Done\n";
?>
--EXPECTF--
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(5) "text1"
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(5) "text1"
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(5) "text1"
}
Done
