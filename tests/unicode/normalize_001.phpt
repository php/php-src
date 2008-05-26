--TEST--
Unicode identifiers normalization (${})
--FILE--
<?php
${"\u212B"} = "ok\n";
echo ${"\u00C5"};
?>
--EXPECT--
ok
