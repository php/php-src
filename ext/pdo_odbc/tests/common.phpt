--TEST--
ODBC
--SKIPIF--
<?php
if (!extension_loaded('pdo_odbc')) print 'skip';
if (substr(PHP_OS, 0, 3) == 'WIN' &&
	false === getenv('PDOTEST_DSN') &&
	false === getenv('PDO_ODBC_TEST_DSN') &&
	!extension_loaded('com_dotnet')) {
	die('skip - either PDOTEST_DSN or com_dotnet extension is needed to setup the connection');
}
--REDIRECTTEST--
# magic auto-configuration

$config = array(
	'TESTS' => 'ext/pdo/tests',
	'ENV' => array()
);

// try loading PDO driver using ENV vars and if none given, and on Windows, try using MS Access
// and if not, skip the test
//
// try to use common PDO env vars, instead of PDO_ODBC specific
if (false !== getenv('PDOTEST_DSN')) {
	// user should have to set PDOTEST_DSN so that:
	// 1. test is skipped if user doesn't want to test it, even if they have MS Access installed
	// 2. it detects if ODBC driver is not installed - to avoid test bug
	// 3. it detects if ODBC driver is installed - so test will be run
	// 4. so a specific ODBC driver can be tested - if system has multiple ODBC drivers

	$config['ENV']['PDOTEST_DSN'] = getenv('PDOTEST_DSN');
	$config['ENV']['PDOTEST_USER'] = getenv('PDOTEST_USER');
	$config['ENV']['PDOTEST_PASS'] = getenv('PDOTEST_PASS');
	if (false !== getenv('PDOTEST_ATTR')) {
		$config['ENV']['PDOTEST_ATTR'] = getenv('PDOTEST_ATTR');
	}
} else if (false !== getenv('PDO_ODBC_TEST_DSN')) {
	// user set these from their shell instead
	$config['ENV']['PDOTEST_DSN'] = getenv('PDO_ODBC_TEST_DSN');
	$config['ENV']['PDOTEST_USER'] = getenv('PDO_ODBC_TEST_USER');
	$config['ENV']['PDOTEST_PASS'] = getenv('PDO_ODBC_TEST_PASS');
	if (false !== getenv('PDO_ODBC_TEST_ATTR')) {
		$config['ENV']['PDOTEST_ATTR'] = getenv('PDO_ODBC_TEST_ATTR');
	}
} elseif (preg_match('/^WIN/i', PHP_OS)) {
	// on Windows and user didn't set PDOTEST_DSN, try this as a fallback:
	// check if MS Access DB is installed, and if yes, try using it. create a temporary MS access database.
	//
	$path = realpath(__DIR__) . '\pdo_odbc.mdb';
	if (!file_exists($path)) {
		try {
			// try to create database
			$adox = new COM('ADOX.Catalog');
			$adox->Create('Provider=Microsoft.Jet.OLEDB.4.0;Data Source=' . $path);
			$adox = null;

		} catch (Exception $e) {
		}
	}
	if (file_exists($path)) {
		// database was created and written to file system
		$config['ENV']['PDOTEST_DSN'] = "odbc:Driver={Microsoft Access Driver (*.mdb)};Dbq=$path;Uid=Admin";
	} // else: $config['ENV']['PDOTEST_DSN'] not set
} // else: $config['ENV']['PDOTEST_DSN'] not set
//         test will be skipped. see SKIPIF section of long_columns.phpt

# other magic autodetection here, eg: for DB2 by inspecting env
/*
$USER = 'db2inst1';
$PASSWD = 'ibmdb2';
$DBNAME = 'SAMPLE';

$CONNECTION = "odbc:DSN=$DBNAME;UID=$USER;PWD=$PASSWD;";
*/


return $config;
