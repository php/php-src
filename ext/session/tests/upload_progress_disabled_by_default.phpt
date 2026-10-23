--TEST--
session.upload_progress.enabled is disabled by default
--EXTENSIONS--
session
--FILE--
<?php
var_dump(ini_get('session.upload_progress.enabled'));
?>
--EXPECT--
string(1) "0"
