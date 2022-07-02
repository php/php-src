--TEST--
SimpleXml: foreach by reference
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = <<<XML
<people>
  <person>Lucy</person>
  <person>Mikasa</person>
</people>
XML;

$people = simplexml_load_string($xml);

foreach ($people as &$person) {}

?>
--EXPECTF--
Fatal error: Uncaught Error: An iterator cannot be used with foreach by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
