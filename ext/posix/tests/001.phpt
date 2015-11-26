--TEST--
posix_access() with bogus paths
--SKIPIF--
<?php if (!extension_loaded('posix')) echo 'skip'; ?>
--FILE--
<?php

var_dump(posix_access(str_repeat('bogus path', 1042)));

?>
--EXPECT--
bool(false)
