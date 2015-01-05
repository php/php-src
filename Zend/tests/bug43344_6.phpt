--TEST--
Bug #43344.6 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
echo namespace\bar."\n";
?>
--EXPECTF--
Fatal error: Undefined constant 'Foo\bar' in %sbug43344_6.php on line %d
