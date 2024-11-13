--TEST--
ReflectionExtension::getDependencies()
--EXTENSIONS--
dom
--FILE--
<?php
$ext = new ReflectionExtension("dom");
$deps = $ext->getDependencies();
var_dump($deps);
?>
--EXPECT--
array(2) {
  ["libxml"]=>
  string(8) "Required"
  ["domxml"]=>
  string(9) "Conflicts"
}
