--TEST--
SimpleXML: adding/removing attributes (second)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 
$xml =<<<EOF
<people>
   <person name="Joe"></person>
   <person name="Boe"></person>
</people>
EOF;

$people = simplexml_load_string($xml);
var_dump($people->person[0]['name']);
var_dump($people->person[1]['age']);
$person = $people->person[1];
$person['name'] = "XXX";
var_dump($people->person[1]['name']);
$people->person[1]['age'] = 30;
var_dump($people->person[1]['age']);
$people->person[1]['age'] += 5;
var_dump($people->person[1]['age']);
echo "---Unset:---\n";
unset($people->person[1]['age']);
echo "---Unset?---\n";
var_dump($people->person[1]['age']);
var_dump(isset($people->person[1]['age']));
echo "---Done---\n";
?>
--EXPECT--
string(3) "Joe"
NULL
string(3) "XXX"
string(2) "30"
string(2) "35"
---Unset:---
---Unset?---
NULL
bool(false)
---Done---
