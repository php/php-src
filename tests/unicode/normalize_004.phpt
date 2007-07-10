--TEST--
Unicode identifiers normalization ($GLOBALS[])
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--FILE--
<?php
$GLOBALS["\u212B"] = "ok\n";
echo $GLOBALS["\u00C5"];
?>
--EXPECT--
ok
