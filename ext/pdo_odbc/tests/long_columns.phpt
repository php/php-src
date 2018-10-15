--TEST--
PDO ODBC "long" columns
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo_odbc')) print 'skip not loaded';
// make sure there is an ODBC driver and a DSN, or the test will fail
include 'ext/pdo/tests/pdo_test.inc';
$config = PDOTest::get_config('ext/pdo_odbc/tests/common.phpt');
if (!isset($config['ENV']['PDOTEST_DSN']) || $config['ENV']['PDOTEST_DSN']===false) print 'skip';
?>
--FILE--
<?php
// setup: set PDOTEST_DSN environment variable
//        for MyODBC (MySQL) and MS SQL Server, you need to also set PDOTEST_USER and PDOTEST_PASS
//
// can use MS SQL Server on Linux - using unixODBC
//   -RHEL6.2
//   -download & instructions: http://www.microsoft.com/en-us/download/details.aspx?id=28160
//      -Linux6\sqlncli-11.0.1790.0.tar.gz (it calls RHEL6.x 'Linux6' for some reason)
//   -follow instructions on web page and install script
//   -may have to specify connection info in connection string without using a DSN (DSN-less connection)
//      -for example:
//            set PDOTEST_DSN='odbc:Driver=SQL Server Native Client 11.0;Server=10.200.51.179;Database=testdb'
//            set PDOTEST_USER=sa
//            set PDOTEST_PASS=Password01
//
// on Windows, the easy way to do this:
// 1. install MS Access (part of MS Office) and include ODBC (Development tools feature)
//       install the x86 build of the Drivers. You might not be able to load the x64 drivers.
// 2. in Control Panel, search for ODBC and open "Setup data sources (ODBC)"
// 3. click on System DSN tab
// 4. click Add and choose "Microsoft Access Driver (*.mdb, *.accdb)" driver
// 5. enter a DSN, ex: accdb12
// 6. click 'Create' and select a file to save the database as
//       -otherwise, you'll have to open MS Access, create a database, then load that file in this Window to map it to a DSN
// 7. set the environment variable PDOTEST_DSN="odbc:<system dsn from step 5>" ex: SET PDOTEST_DSN=odbc:accdb12
//         -note: on Windows, " is included in environment variable
//
// easy way to compile:
// configure --disable-all --enable-cli --enable-zts --enable-pdo --with-pdo-odbc --enable-debug
// configure --disable-all --eanble-cli --enable-pdo --with-pdo-odbc=unixODBC,/usr,/usr --with-unixODBC=/usr --enable-debug
//

require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_odbc/tests/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

if (false === $db->exec('CREATE TABLE TEST (id INT NOT NULL PRIMARY KEY, data CLOB)')) {
	if (false === $db->exec('CREATE TABLE TEST (id INT NOT NULL PRIMARY KEY, data longtext)')) {
		if (false === $db->exec('CREATE TABLE TEST (id INT NOT NULL PRIMARY KEY, data varchar(4000))')) {
			die("BORK: don't know how to create a long column here:\n" . implode(", ", $db->errorInfo()));
		}
	}
}

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

// the driver reads columns in blocks of 255 bytes and then reassembles those blocks into a single buffer.
// test sizes around 255 to make sure that the reassembly works (and that the column is split into 255 byte blocks by the database)
// also, test sizes below 255 to make sure that they work - and are not treated as a long column (should be read in a single read)
$sizes = array(32, 53, 64, 79, 128, 253, 254, 255, 256, 257, 258, 1022, 1023, 1024, 1025, 1026, 510, 511, 512, 513, 514, 1278, 1279, 1280, 1281, 1282, 2046, 2047, 2048, 2049, 2050, 1534, 1535, 1536, 1537, 1538, 3070, 3071, 3072, 3073, 3074, 3998, 3999, 4000);

function alpha_repeat($len) {
	// use the alphabet instead of 'i' characters to make sure the blocks don't overlap when they are reassembled
	$out = "";
	while (strlen($out) < $len) {
		$out .= "abcdefghijklmnopqrstuvwxyz";
	}
	return substr($out, 0, $len);
}

// don't use Prepared Statements. that fails on MS SQL server (works with Access, MyODBC), which is a separate failure, feature/code-path from what
// this test does - nice to be able to test using MS SQL server
foreach ($sizes as $num) {
	$text = alpha_repeat($num);
	$db->exec("INSERT INTO TEST VALUES($num, '$text')");
}

// verify data
foreach ($db->query('SELECT id, data from TEST ORDER BY LEN(data) ASC') as $row) {
	$expect = alpha_repeat($row[0]);
	if (strcmp($expect, $row[1])) {
		echo "Failed on size $row[id]:\n";
		printf("Expected %d bytes, got %d\n", strlen($expect), strlen($row['data']));
		echo ($expect) . "\n";
		echo ($row['data']) . "\n";
	} else {
		echo "Passed on size $row[id]\n";
	}
}

echo "Finished\n";
--EXPECT--
Passed on size 32
Passed on size 53
Passed on size 64
Passed on size 79
Passed on size 128
Passed on size 253
Passed on size 254
Passed on size 255
Passed on size 256
Passed on size 257
Passed on size 258
Passed on size 510
Passed on size 511
Passed on size 512
Passed on size 513
Passed on size 514
Passed on size 1022
Passed on size 1023
Passed on size 1024
Passed on size 1025
Passed on size 1026
Passed on size 1278
Passed on size 1279
Passed on size 1280
Passed on size 1281
Passed on size 1282
Passed on size 1534
Passed on size 1535
Passed on size 1536
Passed on size 1537
Passed on size 1538
Passed on size 2046
Passed on size 2047
Passed on size 2048
Passed on size 2049
Passed on size 2050
Passed on size 3070
Passed on size 3071
Passed on size 3072
Passed on size 3073
Passed on size 3074
Passed on size 3998
Passed on size 3999
Passed on size 4000
Finished
