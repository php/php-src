--TEST--
ODBC
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo_odbc')) print 'skip'; ?>
--REDIRECTTEST--
# magic auto-configuration

$config = array(
	'TESTS' => 'ext/pdo/tests'
);
	

if (false !== getenv('PDO_ODBC_TEST_DSN')) {
	# user set them from their shell
	$config['ENV']['PDOTEST_DSN'] = getenv('PDO_ODBC_TEST_DSN');
	$config['ENV']['PDOTEST_USER'] = getenv('PDO_ODBC_TEST_USER');
	$config['ENV']['PDOTEST_PASS'] = getenv('PDO_ODBC_TEST_PASS');
	if (false !== getenv('PDO_ODBC_TEST_ATTR')) {
		$config['ENV']['PDOTEST_ATTR'] = getenv('PDO_ODBC_TEST_ATTR');
	}
} elseif (preg_match('/^WIN/i', PHP_OS)) {
	# on windows, try to create a temporary MS access database
	$path = realpath(dirname(__FILE__)) . '\pdo_odbc.mdb';
	if (!file_exists($path)) {
		try {
			$adox = new COM('ADOX.Catalog');
			$adox->Create('Provider=Microsoft.Jet.OLEDB.4.0;Data Source=' . $path);
			$adox = null;
	
		} catch (Exception $e) {
		}
	}
	if (file_exists($path)) {
		$config['ENV']['PDOTEST_DSN'] = "odbc:Driver={Microsoft Access Driver (*.mdb)};Dbq=$path;Uid=Admin";
	}
}
# other magic autodetection here, eg: for DB2 by inspecting env
/*
$USER = 'db2inst1';
$PASSWD = 'ibmdb2';
$DBNAME = 'SAMPLE';

$CONNECTION = "odbc:DSN=$DBNAME;UID=$USER;PWD=$PASSWD;";
*/

	
return $config;
