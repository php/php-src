--TEST--
SimpleXML: accessing singular subnode as array
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
var_dump($people->person[0]['name']);
$people->person['name'] = "XXX";
var_dump($people->person['name']);
var_dump($people->person[0]['name']);
$people->person[0]['name'] = "YYY";
var_dump($people->person['name']);
var_dump($people->person[0]['name']);
unset($people->person[0]['name']);
var_dump($people->person['name']);
var_dump($people->person[0]['name']);
var_dump(isset($people->person['name']));
var_dump(isset($people->person[0]['name']));
echo "---Done---\n";
?>
--EXPECT--
string(3) "Joe"
string(3) "Joe"
string(3) "XXX"
string(3) "XXX"
string(3) "YYY"
string(3) "YYY"
NULL
NULL
bool(false)
bool(false)
---Done---
