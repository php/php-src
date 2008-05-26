--TEST--
Unicode identifiers normalization ($$)
--FILE--
<?php
$a = "\u212B";
$b = "\u00C5";
$$a = "ok\n";
echo $$b;
?>
--EXPECT--
ok
