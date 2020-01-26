--TEST--
Bug #78297: Include unexistent file memory leak
--PHPDBG--
r
q
--FILE--
<?php
include "does_not_exist.php";
--EXPECTF--
[Successful compilation of %s]
prompt> 
Warning: include(%s): Failed to open stream: No such file or directory in %s on line %d

Warning: include(): Failed opening 'does_not_exist.php' for inclusion (include_path=%s) in %s on line %d
[Script ended normally]
prompt> 
