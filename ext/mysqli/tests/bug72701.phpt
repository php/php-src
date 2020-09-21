--TEST--
Bug #72701 mysqli_get_host_info() wrong output
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once("connect.inc");

if ("127.0.0.1" != $host && "localhost" != $host) {
    die("skip require 127.0.0.1 connection");
}

?>
--FILE--
<?php

require_once("connect.inc");

$con = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

if (mysqli_connect_errno()) {
    echo "Failed to connect to MySQL: " . mysqli_connect_error();
}

var_dump(preg_match(",(127.0.0.1|localhost) via .*,i", mysqli_get_host_info($con)));

mysqli_close($con);
?>
--EXPECT--
int(1)
