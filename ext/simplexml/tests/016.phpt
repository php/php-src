--TEST--
SimpleXML: modifying attributes of singular subnode
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
$people->person[0]['name'] .= "ZZZ";
var_dump($people->person['name']);
var_dump($people->person[0]['name']);
echo "---Done---\n";
?>
--EXPECT--
string(3) "Joe"
string(3) "Joe"
string(3) "JoeZZZ"
string(3) "JoeZZZ"
---Done---
