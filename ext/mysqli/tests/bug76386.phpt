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
try {
    $link->query(
        'CREATE TABLE IF NOT EXISTS ts_test (id bigint unsigned auto_increment ' .
        'primary key, ts timestamp default 0, ts2 timestamp(2) default 0, ' . 
        'ts4 timestamp(4) default 0, ts6 timestamp(6) default 0) character ' .
        'set utf8 collate utf8_general_ci;'
    );
    $link->query('TRUNCATE TABLE ts_test;');
    $link->query('INSERT INTO ts_test VALUES ();');
    $stmt = $link->prepare('SELECT * FROM ts_test;'); // must be statement
    $stmt->execute();
    $id = $ts = $ts2 = $ts4 = $ts6 = null;
    $stmt->bind_result($id, $ts, $ts2, $ts4, $ts6);
    $stmt->fetch();
    var_dump($ts, $ts2, $ts4, $ts6);
} finally {
    $link->query('DROP TABLE IF EXISTS ts_test;');
    $link->close();
}
?>
--EXPECTF--
string(19) "0000-00-00 00:00:00"
string(22) "0000-00-00 00:00:00.00"
string(24) "0000-00-00 00:00:00.0000"
string(26) "0000-00-00 00:00:00.000000"
