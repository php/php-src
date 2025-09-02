--TEST--
Bug #43344.2 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
echo Foo::bar."\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "Foo\Foo" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %sbug43344_2.php on line %d
