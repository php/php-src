--TEST--
posix_getrlimit(): Basic tests
--SKIP--
<?php if (!function_exists('posix_getrlimit')) die('skip posix_getrlimit() not found'); ?>
--FILE--
<?php

var_dump(posix_getrlimit());

?>
--EXPECTF--
array(%d) {
%a
}
