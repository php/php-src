--TEST--
GH-10983 (State-dependant segfault in ReflectionObject::getProperties)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = <<<XML
<form name="test"></form>
XML;

$simplexml = simplexml_load_string($xml);

var_dump($simplexml['name']);
$reflector = new ReflectionObject($simplexml['name']);
$rprops = $reflector->getProperties();

?>
--EXPECT--
object(SimpleXMLElement)#2 (1) {
  [0]=>
  string(4) "test"
}
