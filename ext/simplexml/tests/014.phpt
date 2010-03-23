--TEST--
SimpleXML: adding/removing attributes (direct)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 
$xml =<<<EOF
<people>
   <person name="Joe"></person>
</people>
EOF;

$people = simplexml_load_string($xml);
var_dump($people->person['name']);
var_dump($people->person['age']);
$person = $people->person;
$person['name'] = "XXX";
var_dump($people->person['name']);
$people->person['age'] = 30;
var_dump($people->person['age']);
echo "---Unset:---\n";
unset($people->person['age']);
echo "---Unset?---\n";
var_dump($people->person['age']);
var_dump(isset($people->person['age']));
$people->person['age'] = 30;
echo "---Unsupported---\n";
var_dump($people->person['age']);
$people->person['age'] += 5;
var_dump($people->person['age']);
?>
===DONE===
--EXPECTF--
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(3) "Joe"
}
NULL
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(3) "XXX"
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(2) "30"
}
---Unset:---
---Unset?---
NULL
bool(false)
---Unsupported---
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(2) "30"
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(2) "35"
}
===DONE===
