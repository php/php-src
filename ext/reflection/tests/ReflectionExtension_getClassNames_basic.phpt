--TEST--
ReflectionExtension::getClassNames() method on an extension which actually returns some information
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
$standard = new ReflectionExtension('standard');
var_dump($standard->getClassNames());
?>
==DONE==
--EXPECTF--
array(3) {
  [0]=>
  %s(22) "__PHP_Incomplete_Class"
  [1]=>
  %s(15) "php_user_filter"
  [2]=>
  %s(9) "Directory"
}
==DONE==
