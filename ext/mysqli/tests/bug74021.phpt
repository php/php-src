--TEST--
Bug #74021 (fetch_array broken data. Data more then MEDIUMBLOB)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

require_once("connect.inc");
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

$link->query('SET @@global.max_allowed_packet = 67108864');
$link->close();

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$res = $link->query("SELECT RPAD('1',9000000,'1') as a,RPAD('1',9000000,'1') as b");
$r = $res->fetch_array();
var_dump(md5($r['a']));
var_dump(md5($r['b']));
?>
--EXPECT--
string(32) "42ca0fd16ab6b6d4b9d47dc0a4a8b12a"
string(32) "42ca0fd16ab6b6d4b9d47dc0a4a8b12a"
