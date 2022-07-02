--TEST--
ReflectionExtension::getClassNames() method on an extension which actually returns some information
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
$standard = new ReflectionExtension('standard');
var_dump($standard->getClassNames());
?>
--EXPECT--
array(4) {
  [0]=>
  string(22) "__PHP_Incomplete_Class"
  [1]=>
  string(14) "AssertionError"
  [2]=>
  string(15) "php_user_filter"
  [3]=>
  string(9) "Directory"
}
