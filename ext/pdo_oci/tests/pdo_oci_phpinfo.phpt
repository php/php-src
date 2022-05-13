--TEST--
PDO_OCI: phpinfo() output
--EXTENSIONS--
pdo
pdo_oci
--SKIPIF--
<?php
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
$db = PDOTest::factory();

ob_start();
phpinfo();
$tmp = ob_get_contents();
ob_end_clean();

$reg = 'PDO Driver for OCI 8 and later => enabled';
if (!preg_match("/$reg/", $tmp)) {
    printf("[001] Cannot find OCI PDO driver line in phpinfo() output\n");
}

print "done!";
?>
--EXPECT--
done!
