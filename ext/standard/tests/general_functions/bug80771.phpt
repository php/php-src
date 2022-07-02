--TEST--
Bug #80771 (phpinfo(INFO_CREDITS) displays nothing in CLI)
--FILE--
<?php
ob_start();
phpinfo(INFO_CREDITS);
$info = ob_get_clean();

ob_start();
phpcredits();
$credits = ob_get_clean();

var_dump(strpos($info, $credits) !== false);
?>
--EXPECT--
bool(true)
