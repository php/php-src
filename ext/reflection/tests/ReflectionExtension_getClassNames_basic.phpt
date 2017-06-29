--TEST--
ReflectionExtension::getClassNames() method on an extension which actually returns some information
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
Richard Fussenegger <php@fleshgrinder.com>
--FILE--
<?php
$standard = new ReflectionExtension('standard');
var_dump($standard->getClassNames());
?>
--EXPECT--
array(6) {
  [0]=>
  string(22) "__PHP_Incomplete_Class"
  [1]=>
  string(15) "php_user_filter"
  [2]=>
  string(9) "Directory"
  [3]=>
  string(14) "AssertionError"
  [4]=>
  string(12) "PHP\Std\UUID"
  [5]=>
  string(26) "PHP\Std\UUIDParseException"
}
