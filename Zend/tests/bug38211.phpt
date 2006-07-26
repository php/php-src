--TEST--
Bug #38211 (variable name and cookie name match breaks script execution)
--FILE--
<?php
$test = 'test';
unset($$test);
echo "ok\n";
?>
--EXPECT--
ok
