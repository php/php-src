--TEST--
Test posix_getgroups() function : basic functionality
--EXTENSIONS--
posix
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
