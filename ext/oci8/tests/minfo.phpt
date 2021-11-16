--TEST--
Code coverage for PHP_MINFO_FUNCTION(oci)
--EXTENSIONS--
oci8
--FILE--
<?php

ob_start();
phpinfo(INFO_MODULES);
$v = ob_get_clean();
$r = preg_match('/OCI8 Support .* enabled/', $v);
if ($r !== 1)
    var_dump($r);

echo "Done\n";

?>
--EXPECT--
Done
