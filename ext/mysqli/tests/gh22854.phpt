--TEST--
GH-22854 (Assertion failure at link_errno_read in ext/mysqli/mysqli_prop.c)
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

function read_properties(mysqli $mysqli, array $properties): void {
    foreach ($properties as $property) {
        try {
            $mysqli->$property;
            echo "$property: no error\n";
        } catch (Error $e) {
            echo "$property: " . $e->getMessage() . "\n";
        }
    }

    // var_dump() reads every property in quiet mode
    ob_start();
    var_dump($mysqli);
    ob_end_clean();
    echo "var_dump() done\n";
}

$links = mysqli_get_links_stats()['total'];

echo "connect()\n";
$mysqli = new mysqli();
$mysqli->connect($host, $user, $passwd, $db, $port, $socket);
echo 'Success... ' . $mysqli->host_info . "\n";
try {
    $mysqli->connect($host, $user, $passwd, $db.'wrong', $port, $socket);
} catch (mysqli_sql_exception $e) {
    echo "Error: " . $e->getMessage() . "\n";
}
read_properties($mysqli, ['host_info', 'affected_rows', 'error_list', 'errno', 'error']);
unset($mysqli);

echo "\nreal_connect()\n";
$mysqli = new mysqli();
$mysqli->connect($host, $user, $passwd, $db, $port, $socket);
echo 'Success... ' . $mysqli->host_info . "\n";
try {
    $mysqli->real_connect($host, $user, $passwd, $db.'wrong', $port, $socket);
} catch (mysqli_sql_exception $e) {
    echo "Error: " . $e->getMessage() . "\n";
}
read_properties($mysqli, ['host_info', 'server_info', 'affected_rows', 'error_list']);
unset($mysqli);

// A failed reconnect must leave the link count balanced (it used to be decremented twice)
var_dump(mysqli_get_links_stats()['total'] === $links);

print "done!";
?>
--EXPECTF--
connect()
Success... %s via %s
Error: Unknown database '%s'
host_info: Property access is not allowed yet
affected_rows: Property access is not allowed yet
error_list: Property access is not allowed yet
errno: mysqli object is not fully initialized
error: mysqli object is not fully initialized
var_dump() done

real_connect()
Success... %s via %s
Error: Unknown database '%s'
host_info: Property access is not allowed yet
server_info: Property access is not allowed yet
affected_rows: Property access is not allowed yet
error_list: Property access is not allowed yet
var_dump() done
bool(true)
done!
