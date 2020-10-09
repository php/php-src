--TEST--
OCI
--SKIPIF--
<?php
if (!extension_loaded('pdo_oci')) print 'skip'; ?>
--REDIRECTTEST--
# magic auto-configuration

$config = array(
	'TESTS' => 'ext/pdo/tests'
);


if (false !== getenv('PDO_OCI_TEST_DSN')) {
	# user set them from their shell
	$config['ENV']['PDOTEST_DSN'] = getenv('PDO_OCI_TEST_DSN');
	$config['ENV']['PDOTEST_USER'] = getenv('PDO_OCI_TEST_USER');
	$config['ENV']['PDOTEST_PASS'] = getenv('PDO_OCI_TEST_PASS');
	$config['ENV']['PDOTEST_ATTR'] = getenv('PDO_OCI_TEST_ATTR');
} else {
	$config['ENV']['PDOTEST_DSN'] = 'oci:dbname=localhost/xe;charset=WE8MSWIN1252';
	$config['ENV']['PDOTEST_USER'] = 'SYSTEM';
	$config['ENV']['PDOTEST_PASS'] = 'oracle';
}

return $config;
