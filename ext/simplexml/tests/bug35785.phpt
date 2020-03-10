--TEST--
Bug #35785 (SimpleXML memory read error)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml = simplexml_load_string("<root></root>");
$xml->bla->posts->name = "FooBar";
echo $xml->asXML();
$xml = simplexml_load_string("<root></root>");
var_dump(isset($xml->bla->posts));
$xml->bla->posts[0]->name = "FooBar";
echo $xml->asXML();
?>
--EXPECTF--
Fatal error: Uncaught Error: Attempt to assign property 'name' of non-object in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
