--TEST--
Test posix_getgrnam() function : basic functionality
--SKIPIF--
<?php
    if (!extension_loaded('posix')) die('skip - POSIX extension not loaded');
    if (!posix_getgroups()) die('skip - groups unavailable (ci)');
?>
--FILE--
<?php
  $groupid = posix_getgroups()[0];
  $group = posix_getgrgid($groupid);
  $groupinfo = posix_getgrnam($group["name"]);
  var_dump($groupinfo);
  $groupinfo = posix_getgrnam("");
  var_dump($groupinfo);
?>
--EXPECTF--
array(4) {
  ["name"]=>
  string(%d) "%s"
  ["passwd"]=>
  string(1) "%s"
  ["members"]=>
%a
  ["gid"]=>
  int(%d)
}
bool(false)
