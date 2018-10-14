--TEST--
SimpleXML: foreach and count
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml =<<<EOF
<people>
  <person name="Joe"/>
  <person name="John">
    <children>
      <person name="Joe"/>
    </children>
  </person>
  <person name="Jane"/>
</people>
EOF;

$people = simplexml_load_string($xml);

foreach($people as $person)
{
	var_dump((string)$person['name']);
	var_dump(count($people));
	var_dump(count($person));
}

?>
===DONE===
--EXPECT--
string(3) "Joe"
int(3)
int(0)
string(4) "John"
int(3)
int(1)
string(4) "Jane"
int(3)
int(0)
===DONE===
