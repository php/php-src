--TEST--
posix_getrlimit() for single resource
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (getenv('SKIP_REPEAT')) die('skip Not repeatable');
if (!function_exists('posix_getrlimit')) die('skip posix_getrlimit() not found');
if (!function_exists('posix_setrlimit') || !posix_setrlimit(POSIX_RLIMIT_CORE, 2048, -1)) {
    die('skip Failed to set POSIX_RLIMIT_CORE');
}
?>
--FILE--
<?php

posix_setrlimit(POSIX_RLIMIT_CORE, 2048, -1);
[$hard, $soft] = posix_getrlimit(POSIX_RLIMIT_CORE);
var_dump($hard, $soft);

posix_setrlimit(POSIX_RLIMIT_CORE, 1024, 2048);
[$hard, $soft] = posix_getrlimit(POSIX_RLIMIT_CORE);
var_dump($hard, $soft);

?>
--EXPECT--
int(2048)
string(9) "unlimited"
int(1024)
int(2048)
