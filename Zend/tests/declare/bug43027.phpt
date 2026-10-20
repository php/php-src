--TEST--
Bug #43027 (Declare cause fatal error)
--FILE--
<?php
declare(ticks=1);
namespace test;
echo "ok\n";
?>
--EXPECT--
ok
