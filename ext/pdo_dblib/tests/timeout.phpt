--TEST--
PDO_DBLIB: Set query timeouts
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

$sql = 'WAITFOR DELAY \'00:00:02\'';

// querying without a timeout will succeed
$stmt = $db->prepare($sql);
if ($stmt->execute()) {
	echo "OK\n";
}

// regular timeout attribute will affect query timeout, causing this query to fail
$db = new PDO($dsn, $user, $pass, [PDO::ATTR_TIMEOUT => 1]);
$stmt = $db->prepare($sql);
if (!$stmt->execute()) {
	echo "OK\n";

	// expect some kind of error code
	if ($stmt->errorCode() != '00000') {
		echo "OK\n";
	}
}

// pdo_dblib-specific timeout attribute will control query timeout, causing this query to fail
$db = new PDO($dsn, $user, $pass, [PDO::DBLIB_ATTR_QUERY_TIMEOUT => 1]);
$stmt = $db->prepare($sql);
if (!$stmt->execute()) {
	echo "OK\n";

	// expect some kind of error code
	if ($stmt->errorCode() != '00000') {
		echo "OK\n";
	}
}

?>
--EXPECT--
OK
OK
OK
OK
OK
