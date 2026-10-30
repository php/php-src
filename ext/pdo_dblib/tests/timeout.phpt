--TEST--
PDO_DBLIB: Set query timeouts
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
require __DIR__ . '/config.inc';
getDbConnection();
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$db = getDbConnection();

$sql = 'WAITFOR DELAY \'00:00:02\'';

// regular timeout attribute, set after instance created, will affect query timeout, causing this query to fail
$db = getDbConnection();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$db->setAttribute(PDO::ATTR_TIMEOUT, 1);
$stmt = $db->prepare($sql);
if (!$stmt->execute()) {
    echo "OK\n";

    // expect some kind of error code
    if ($stmt->errorCode() != '00000') {
        echo "OK\n";
    }
}

// pdo_dblib-specific timeout attribute, set after instance created, will control query timeout, causing this query to fail
$db = getDbConnection();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$db->setAttribute(PDO::DBLIB_ATTR_QUERY_TIMEOUT, 1);
$stmt = $db->prepare($sql);
if (!$stmt->execute()) {
    echo "OK\n";

    // expect some kind of error code
    if ($stmt->errorCode() != '00000') {
        echo "OK\n";
    }
}

// regular timeout attribute will affect query timeout, causing this query to fail
$db = getDbConnection(PDO::class, [PDO::ATTR_ERRMODE => PDO::ERRMODE_SILENT, PDO::ATTR_TIMEOUT => 1]);
$stmt = $db->prepare($sql);
if (!$stmt->execute()) {
    echo "OK\n";

    // expect some kind of error code
    if ($stmt->errorCode() != '00000') {
        echo "OK\n";
    }
}

// pdo_dblib-specific timeout attribute will control query timeout, causing this query to fail
$db = getDbConnection(PDO::class, [PDO::ATTR_ERRMODE => PDO::ERRMODE_SILENT, PDO::DBLIB_ATTR_QUERY_TIMEOUT => 1]);
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
OK
OK
OK
