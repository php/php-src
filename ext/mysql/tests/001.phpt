--TEST--
mysql connect
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'connect.inc';
$test = '';

/*** test mysql_connect 127.0.0.1 ***/
$db = mysql_connect('127.0.0.1', $user, $passwd);
$test .= ($db) ? '1' : '0';
mysql_close($db);

/*** test mysql_connect localhost ***/
$db = mysql_connect($host, $user, $passwd);
$test .= ($db) ? '1' : '0';
mysql_close($db);

/*** test mysql_connect localhost:port ***/
$db = mysql_connect($host, $user, $passwd, '', 3306);
$test .= ($db) ? '1' : '0';
mysql_close($db);

var_dump($test);

?>
--EXPECT--
string(3) "111"
