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
$link->query(
    'CREATE TABLE ts_test (id bigint unsigned auto_increment primary key, ' .
    'ts timestamp default current_timestamp(), ts2 timestamp(2) default ' .
    'current_timestamp(2), ts4 timestamp(4) default current_timestamp(4), ' .
    'ts6 timestamp(6) default current_timestamp(6)) character set utf8 collate ' .
    'utf8_general_ci;'
);
$link->query(
    'INSERT INTO ts_test (ts, ts2, ts4, ts6) VALUES ("2018-01-01 11:22:33", ' .
    '"2018-02-02 11:22:33.77", "2018-03-03 11:22:33.8888", ' .
    '"2018-04-04 11:22:33.999999");'
);
$stmt = $link->prepare('SELECT * FROM ts_test;'); // must be statement
$stmt->execute();
$id = $ts = $ts2 = $ts4 = $ts6 = null;
$stmt->bind_result($id, $ts, $ts2, $ts4, $ts6);
$stmt->fetch();
$link->close();
var_dump($ts, $ts2, $ts4, $ts6);
?>
--EXPECTF--
string(19) "2018-01-01 11:22:33"
string(22) "2018-02-02 11:22:33.77"
string(24) "2018-03-03 11:22:33.8888"
string(26) "2018-04-04 11:22:33.999999"
--CLEAN--
<?php
require_once('connect.inc');
$link = new mysqli($host, $user, $passwd, $db, $port, $socket);
$link->query('DROP TABLE ts_test;');
$link->close();
?>
