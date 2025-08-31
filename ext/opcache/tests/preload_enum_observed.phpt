--TEST--
Enum preloading with observers
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_enum.inc
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php

spl_autoload_register(static function ($class) {
	if ($class === 'MyEnum') {
		require_once(__DIR__ . '/preload_enum.inc');
	}
});

var_dump(MyEnum::cases());

?>
--EXPECTF--
<!-- init '%spreload_enum.inc' -->
<file '%spreload_enum.inc'>
  <!-- init var_dump() -->
  <var_dump>
enum(MyEnum::Bar)
  </var_dump>
</file '%spreload_enum.inc'>
<!-- init '%spreload_enum_observed.php' -->
<file '%spreload_enum_observed.php'>
  <!-- init spl_autoload_register() -->
  <spl_autoload_register>
  </spl_autoload_register>
  <!-- init MyEnum::cases() -->
  <MyEnum::cases>
  </MyEnum::cases>
  <!-- init var_dump() -->
  <var_dump>
array(2) {
  [0]=>
  enum(MyEnum::Foo)
  [1]=>
  enum(MyEnum::Bar)
}
  </var_dump>
</file '%spreload_enum_observed.php'>
