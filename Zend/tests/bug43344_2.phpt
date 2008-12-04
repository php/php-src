--TEST--
Bug #43344.2 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
echo Foo::bar."\n";
?>
--EXPECTF--
Fatal error: Class 'Foo\Foo' not found in %sbug43344_2.php on line %d
