--TEST--
Unicode identifiers normalization (${})
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--FILE--
<?php
${"\u212B"} = "ok\n";
echo ${"\u00C5"};
?>
--EXPECT--
ok
