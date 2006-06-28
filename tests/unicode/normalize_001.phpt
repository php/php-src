--TEST--
Unicode identifiers normalization (${})
--INI--
unicode.semantics=on
--FILE--
<?php
${"\u212B"} = "ok\n";
echo ${"\u00C5"};
?>
--EXPECT--
ok
