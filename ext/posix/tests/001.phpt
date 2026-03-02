--TEST--
posix_access() with bogus paths
--EXTENSIONS--
posix
--FILE--
<?php

var_dump(posix_access(str_repeat('bogus path', 1042)));

?>
--EXPECT--
bool(false)
