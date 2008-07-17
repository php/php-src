--TEST--
Bug #43027 (Declare cause fatal error)
--FILE--
<?php
declare(ticks=1);
namespace test;
echo "ok\n";
?>
--EXPECTF--
Deprecated: Ticks is deprecated and will be removed in PHP 6 in %s on line %d
ok
