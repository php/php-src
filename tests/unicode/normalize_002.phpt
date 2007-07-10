--TEST--
Unicode identifiers normalization ($$)
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--FILE--
<?php
$a = "\u212B";
$b = "\u00C5";
$$a = "ok\n";
echo $$b;
?>
--EXPECT--
ok
