--TEST--
Code coverage for PHP_MINFO_FUNCTION(oci)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

ob_start();
phpinfo(INFO_MODULES);
$v = ob_get_clean();
$r = strpos($v, 'OCI8 Support => enabled');
var_dump($r);

echo "Done\n";

?>
--EXPECTF--
int(%d)
Done
