--TEST--
oss-fuzz #65021
--FILE--
<?php
var_dump(preg_match(">",""));
var_dump(preg_match(">foo>i","FOO"));
?>
--EXPECTF--
Warning: preg_match(): No ending delimiter '>' found in %s on line %d
bool(false)
int(1)
