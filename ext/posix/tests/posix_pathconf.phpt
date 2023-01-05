--TEST--
Test posix_pathconf
--EXTENSIONS--
posix
--FILE--
<?php
var_dump(posix_pathconf('', POSIX_PC_PATH_MAX));
var_dump(posix_pathconf(str_repeat('non_existent', 4096), POSIX_PC_NAME_MAX));
var_dump(posix_errno() != 0);
var_dump(posix_pathconf(sys_get_temp_dir(), POSIX_PC_PATH_MAX));
?>
--EXPECTF--
bool(false)
bool(false)
bool(true)
int(%d)
