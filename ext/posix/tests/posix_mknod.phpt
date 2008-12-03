--TEST--
posix_mknod(): Basic tests
--SKIP--
<?php if (!posix_mknod('posix_mknod')) die('skip posix_mknod() not found'); ?>
--FILE--
<?php

var_dump(posix_mknod(NULL, NULL, NULL, NULL));

?>
--EXPECT--
bool(false)
