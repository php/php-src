--TEST--
ReflectionExtension::getDependencies() method on an extension with a required and conflicting dependency
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new ReflectionExtension('dom');
var_dump($dom->getDependencies());
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
