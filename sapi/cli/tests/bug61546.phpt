--TEST--
Bug #61546 (functions related to current script failed when chdir() in cli sapi)
--FILE--
<?php
// reference doc for getmyinode() on php.net states that it returns an integer or FALSE on error
// on Windows, getmyinode() returns 0 which normally casts to FALSE
// however, the implementation of getmyinode() (in pageinfo.c) returns an explicit FALSE in the
// event that the internal page_inode structure is less than 0, otherwise it returns the long value
// of page_inode. therefore, an explicit 0 should be a passing value for this test.
//
// the ext/standard/tests/file/statpage.phpt test also tests getmyinode() returns an integer and will
// pass even if that integer is 0. on Windows, the getmyinode() call in statpage.phpt returns 0 and
// passes on Windows.
$php = getenv("TEST_PHP_EXECUTABLE_ESCAPED");
$test_code = <<<PHP
<?php
chdir('..');
var_dump(get_current_user() != "");
chdir('..');
var_dump(getmyinode() !== false);
var_dump(getlastmod() != false);
PHP;

file_put_contents("bug61546_sub.php", $test_code);
system($php . ' -n bug61546_sub.php');
unlink("bug61546_sub.php");
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
