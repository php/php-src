--TEST--
GH-7932 (MariaDB version prefix not always stripped)
--SKIPIF--
<?php
require_once 'skipif.inc';
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once "connect.inc";

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

// It seems we can only test the happy path...
if (str_starts_with($mysqli->server_info, '5.5.5-')) {
    print("Expecting stripped prefix. Found: " . $mysqli->server_info . "\n");
}
?>
--EXPECT--
