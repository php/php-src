--TEST--
SimpleXml: foreach by reference
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
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
