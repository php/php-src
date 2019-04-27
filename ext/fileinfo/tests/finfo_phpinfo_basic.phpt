--TEST--
Test finfo extension : loading into phpinfo()
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

echo "*** Testing finfo extension : loading info phpinfo() ***\n";

ob_start();
echo phpinfo(8);
$output = ob_get_clean();

var_dump(preg_match("/fileinfo support => enabled/", $output));

?>
===DONE===
--EXPECT--
*** Testing finfo extension : loading info phpinfo() ***
int(1)
===DONE===
