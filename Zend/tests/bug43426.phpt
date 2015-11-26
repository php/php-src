--TEST--
Bug #43426 (crash on nested call_user_func calls)
--FILE--
<?php
$c = 1; // doesn't matter
call_user_func("foo2", $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c,
$c,
 $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c, $c);
function foo2($d) {}      
echo "ok\n";
?>
--EXPECT--
ok
