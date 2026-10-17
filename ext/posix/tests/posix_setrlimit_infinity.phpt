--TEST--
posix_setrlimit() with POSIX_RLIMIT_INFINITY
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists('posix_setrlimit')) die('skip posix_setrlimit() not found');
if (posix_getrlimit(POSIX_RLIMIT_CORE)[1] !== 'unlimited') die('skip hard limit of POSIX_RLIMIT_CORE is not unlimited');
?>
--FILE--
<?php

var_dump(posix_setrlimit(POSIX_RLIMIT_CORE, 1024, POSIX_RLIMIT_INFINITY));
var_dump(posix_getrlimit(POSIX_RLIMIT_CORE));

var_dump(posix_setrlimit(POSIX_RLIMIT_CORE, POSIX_RLIMIT_INFINITY, POSIX_RLIMIT_INFINITY));
var_dump(posix_getrlimit(POSIX_RLIMIT_CORE));

?>
--EXPECT--
bool(true)
array(2) {
  [0]=>
  int(1024)
  [1]=>
  string(9) "unlimited"
}
bool(true)
array(2) {
  [0]=>
  string(9) "unlimited"
  [1]=>
  string(9) "unlimited"
}
