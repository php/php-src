--TEST--
Bug #75696 (posix_getgrnam fails to print details of group)
--EXTENSIONS--
posix
--FILE--
<?php
$gid = posix_getgid();
$name = posix_getgrgid($gid)['name'];
$info = posix_getgrnam($name);
var_dump(is_array($info));
?>
--EXPECT--
bool(true)
