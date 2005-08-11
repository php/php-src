--TEST--
Unicode identifiers normalization ($$)
--INI--
unicode_semantics=on
--FILE--
<?php
$a = "\u212B";
$b = "\u00C5";
$$a = "ok\n";
echo $$b;
?>
--EXPECT--
ok
