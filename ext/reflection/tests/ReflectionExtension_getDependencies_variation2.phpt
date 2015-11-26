--TEST--
ReflectionExtension::getDependencies() method on an extension with one optional dependency
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
$standard = new ReflectionExtension('standard');
var_dump($standard->getDependencies());
?>
==DONE==
--EXPECTF--
array(1) {
  ["session"]=>
  %s(8) "Optional"
}
==DONE==
