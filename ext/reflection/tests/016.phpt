--TEST--
ReflectionExtension::getDependencies()
--EXTENSIONS--
xml
--FILE--
<?php
$ext = new ReflectionExtension("xml");
$deps = $ext->getDependencies();
var_dump($deps);
?>
--EXPECT--
array(1) {
  ["libxml"]=>
  string(8) "Required"
}
