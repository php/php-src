--TEST--
Bug GH-8267 (Invalid error message when connection via SSL fails)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';

if (!($link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)))
    die(sprintf("skip Connect failed, [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

$res = $link->query("SHOW VARIABLES LIKE 'have_ssl'");
$row = $res ? $res->fetch_row() : null;
// MySQL 8.4+ no longer has have_ssl so the check is only relevant for older versions and MariaDB
if ($row && $row[1] !== 'YES') {
    die('skip Server has no SSL support');
}
?>
--FILE--
<?php
require_once "connect.inc";

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$mysql = mysqli_init();
// Ignore this warning as we are providing wrong information on purpose
mysqli_ssl_set($mysql, 'x509.key', 'x509.pem', 'x509.ca', null, null);
try {
    // There should be no warning here, only exception
    mysqli_real_connect($mysql, $host, $user, $passwd, null, $port, null, MYSQLI_CLIENT_SSL);
} catch (mysqli_sql_exception $e) {
    echo $e->getMessage()."\n";
}

echo 'done!';
?>
--EXPECTF--
Warning: failed loading cafile stream: `x509.ca' in %s
Cannot connect to MySQL using SSL
done!
