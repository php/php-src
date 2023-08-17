--TEST--
Bug GH-8267 (Invalid error message when connection via SSL fails)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
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
