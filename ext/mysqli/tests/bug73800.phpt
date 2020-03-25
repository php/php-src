--TEST--
Bug #73800 (sporadic segfault with MYSQLI_OPT_INT_AND_FLOAT_NATIVE)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
if (PHP_INT_SIZE != 8) die('skip requires 64-bit');
?>
--FILE--
<?php

require_once("connect.inc");
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

$link->query('SET @@global.max_allowed_packet = 67108864');
$link->close();

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$link->options(MYSQLI_OPT_INT_AND_FLOAT_NATIVE, 1);

$res = $link->query("SELECT RPAD('1',9000000,'1') as a,RPAD('1',9000000,'1') as b, 9223372036854775807 as c");
$r = $res->fetch_array();

var_dump($r['c']);
?>
--EXPECT--
int(9223372036854775807)
