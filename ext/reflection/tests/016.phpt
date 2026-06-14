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
array(3) {
  ["libxml"]=>
  string(8) "Required"
  ["lexbor"]=>
  string(8) "Required"
  ["domxml"]=>
  string(9) "Conflicts"
}
