--TEST--
host_info() dumps with assert after failed reconnect
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
include 'connect.inc';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$mysqli = mysqli_init();

$mysqli->connect($host, $user, $passwd, $db, $port, $socket);

echo 'Success... ' . $mysqli->host_info . "\n";

try {
    $mysqli->connect($host, $user, $passwd, $db.'wrong', $port, $socket);
} catch (mysqli_sql_exception $e) {
    echo "Error: " . $e->getMessage() . "\n";
}

try {
    echo $mysqli->host_info . "\n";
} catch (Error $e) {
    echo "Error: " . $e->getMessage() . "\n";
}

print "done!";
?>
--EXPECTF--
Success... %s via %s
Error: Unknown database 'testwrong'
Error: mysqli object is not fully initialized
done!
