--TEST--
Bug #76450 (SIGSEGV in firebird_stmt_execute)
--EXTENSIONS--
pdo_firebird
sockets
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
require_once "payload_server.inc";

$address = run_server(__DIR__ . "/bug_76450.data");

// no need to change the credentials; we're running against a fake server
$dsn = "firebird:dbname=inet://$address/test";
$username = 'SYSDBA';
$password = 'masterkey';

$dbh = new PDO($dsn, $username, $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
$sql = "EXECUTE PROCEDURE test_proc 123";
$query = $dbh->prepare($sql);
try {
    $query->execute();
} catch (Exception $ex) {
    echo "{$ex->getMessage()}\n";
}
?>
--EXPECT--
SQLSTATE[HY000]: General error
