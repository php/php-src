--TEST--
Test for buffering in core functions with implicit flush on
--INI--
implicit_flush=1
--FILE--
<?php
$res = var_export("foo1");
echo "\n";
$res = var_export("foo2", TRUE);
echo "\n";
echo $res."\n";
?>
--EXPECT--
'foo1'

'foo2'
