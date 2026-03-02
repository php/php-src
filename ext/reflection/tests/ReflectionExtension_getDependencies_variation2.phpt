--TEST--
ReflectionExtension::getDependencies() method on an extension with one optional dependency
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
$standard = new ReflectionExtension('standard');
var_dump($standard->getDependencies());
?>
--EXPECT--
array(3) {
  ["random"]=>
  string(8) "Required"
  ["uri"]=>
  string(8) "Required"
  ["session"]=>
  string(8) "Optional"
}
