--TEST--
Prepared Statement formatter truncates fractional seconds from date/time column (bug #76386)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once('connect.inc');
$link = new mysqli($host, $user, $passwd, $db, $port, $socket);
$link->query('DROP TABLE IF EXISTS ts_test;');
$link->query('DROP TABLE IF EXISTS t_test;');
$link->query(
    'CREATE TABLE ts_test (id bigint unsigned auto_increment primary key, ' .
    'ts timestamp default current_timestamp(), ts2 timestamp(2) default ' .
    'current_timestamp(2), ts4 timestamp(4) default current_timestamp(4), ' .
    'ts6 timestamp(6) default current_timestamp(6)) character set utf8 collate ' .
    'utf8_general_ci;'
);
$link->query(
    'CREATE TABLE t_test (id bigint unsigned auto_increment primary key, ' .
    't time default current_time(), t2 time(2) default current_time(2), t4 ' .
    'time(4) default current_time(4), t6 time(6) default current_time(6)) ' .
    'character set utf8 collate utf8_general_ci;'
);
$link->query(
    'INSERT INTO ts_test (ts, ts2, ts4, ts6) VALUES ("2018-01-01 11:22:33", ' .
    '"2018-02-02 11:22:33.77", "2018-03-03 11:22:33.8888", ' .
    '"2018-04-04 11:22:33.999999");'
);
$link->query(
    'INSERT INTO t_test (t, t2, t4, t6) VALUES ("21:22:33", "21:22:33.44", ' .
    '"21:22:33.5555", "21:22:33.676767");'
);

$stmt = $link->prepare('SELECT * FROM ts_test;'); // must be statement
$stmt->execute();
$id = $ts = $ts2 = $ts4 = $ts6 = null;
$stmt->bind_result($id, $ts, $ts2, $ts4, $ts6);
$stmt->fetch();
var_dump($ts, $ts2, $ts4, $ts6);
$stmt->free_result();

$stmt2 = $link->prepare('SELECT * FROM t_test;');
$stmt2->execute();
$id = $t = $t2 = $t3 = $t4 = null;
$stmt2->bind_result($id, $t, $t2, $t4, $t6);
$stmt2->fetch();
var_dump($t, $t2, $t4, $t6);
$stmt2->free_result();

$link->close();
?>
--EXPECTF--
string(19) "2018-01-01 11:22:33"
string(22) "2018-02-02 11:22:33.77"
string(24) "2018-03-03 11:22:33.8888"
string(26) "2018-04-04 11:22:33.999999"
string(8) "21:22:33"
string(11) "21:22:33.44"
string(13) "21:22:33.5555"
string(15) "21:22:33.676767"
--CLEAN--
<?php
require_once('connect.inc');
$link = new mysqli($host, $user, $passwd, $db, $port, $socket);
//$link->query('DROP TABLE ts_test;');
//$link->query('DROP TABLE t_test;');
$link->close();
?>
