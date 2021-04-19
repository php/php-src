--TEST--
Fix #80960 (opendir() warning wrong info when failed on Windows)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip for Windows only");
?>
--INI--
log_errors_max_len=0
--FILE--
<?php
opendir("notexist*");
opendir("notexist?");
opendir(str_pad("longname", PHP_MAXPATHLEN - strlen(getcwd()), "_"));
?>
--EXPECTF--
Warning: opendir(notexist*): %s (code: 123) in %s on line %d

Warning: opendir(notexist*): failed to open dir: No such file or directory in %s on line %d

Warning: opendir(notexist?): %s (code: 123) in %s on line %d

Warning: opendir(notexist?): failed to open dir: No such file or directory in %s on line %d

Warning: opendir(longname%r_+%r): %s (code: 111) in %s on line %d

Warning: opendir(longname%r_+%r): failed to open dir: Filename too long in %s on line %d
