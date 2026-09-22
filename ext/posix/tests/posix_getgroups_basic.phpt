--TEST--
Test posix_getgroups() function : basic functionality
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (getenv("GITHUB_ACTIONS") && PHP_OS_FAMILY === "Darwin") {
    die("flaky Occasionally segfaults on macOS for unknown reasons");
}
?>
--FILE--
<?php
  echo "Basic test of POSIX getgroups\n";

  $groups = posix_getgroups();

  if (!is_array($groups)) {
    echo "TEST FAILED: ", posix_strerror(posix_get_last_error()), "\n";
  } else {
    echo "TEST PASSED\n";
  }

?>
--EXPECT--
Basic test of POSIX getgroups
TEST PASSED
