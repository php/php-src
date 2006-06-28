--TEST--
Unicode identifiers normalization ($GLOBALS[])
--INI--
unicode.semantics=on
--FILE--
<?php
$GLOBALS["\u212B"] = "ok\n";
echo $GLOBALS["\u00C5"];
?>
--EXPECT--
ok
