--TEST--
SimpleXML: foreach and count
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 
$xml =b<<<EOF
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
--EXPECTF--
unicode(3) "Joe"
int(3)
int(0)
unicode(4) "John"
int(3)
int(1)
unicode(4) "Jane"
int(3)
int(0)
===DONE===
