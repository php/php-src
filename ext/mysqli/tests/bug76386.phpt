--TEST--
Prepared Statement formatter truncates fractional seconds from date/time column (bug #76386)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
require_once("connect.inc");

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	die("skip Cannot connect to check required version");

/* Fractional seconds are supported with servers >= 5.6.4. */
if (mysqli_get_server_version($link) < 50604) {
	die(sprintf("skip Server doesn't support fractional seconds in timestamp (%s)", mysqli_get_server_version($link)));
}
mysqli_close($link);
?>
--FILE--
<?php
require_once('connect.inc');
// test part 1 - TIMESTAMP(n)
$link = new mysqli($host, $user, $passwd, $db, $port, $socket);
$link->query('DROP TABLE IF EXISTS ts_test;');
$link->query(
    'CREATE TABLE ts_test (id bigint unsigned auto_increment primary key, ' .
    'ts timestamp default current_timestamp(), ts2 timestamp(2) default ' .
    'current_timestamp(2), ts2b timestamp(2) default "2018-01-01 03:04:05.06", ' .
    'ts4 timestamp(4) default current_timestamp(4), ts4b timestamp(4) default ' .
    '"2018-01-01 03:04:05.0070", ts6 timestamp(6) default current_timestamp(6), ts6b ' .
    'timestamp(6) default "2018-01-01 03:04:05.008000") character set utf8 collate ' .
    'utf8_general_ci;'
);
$link->query(
    'INSERT INTO ts_test (ts, ts2, ts4, ts6) VALUES ("2018-01-01 11:22:33", ' .
    '"2018-02-02 11:22:33.77", "2018-03-03 11:22:33.8888", ' .
    '"2018-04-04 11:22:33.999999");'
);
$stmt = $link->prepare('SELECT * FROM ts_test;'); // must be statement
if ($stmt) {
    $stmt->execute();
    $tsid = $ts = $ts2 = $ts2b = $ts4 = $ts4b = $ts6 = $ts6b = null;
    $stmt->bind_result($tsid, $ts, $ts2, $ts2b, $ts4, $ts4b, $ts6, $ts6b);
    $stmt->fetch();
    var_dump($ts, $ts2, $ts2b, $ts4, $ts4b, $ts6, $ts6b);
    $stmt->free_result();
} else {
    echo('[FIRST][FAIL] mysqli::prepare returned false: ' . $link->error . PHP_EOL);
}
$link->close();

// test part 2 - TIME(n)
$link = new mysqli($host, $user, $passwd, $db, $port, $socket);
$link->query('DROP TABLE IF EXISTS t_test;');
$link->query(
    'CREATE TABLE t_test (id bigint unsigned auto_increment primary key, ' .
    't time default "11:00:00", t2 time(2) default "11:00:00.22", t4 ' .
    'time(4) default "11:00:00.4444", t6 time(6) default "11:00:00.006054") ' .
    'character set utf8 collate utf8_general_ci;'
);
$link->query(
    'INSERT INTO t_test (t, t2, t4, t6) VALUES ("21:22:33", "21:22:33.44", ' .
    '"21:22:33.5555", "21:22:33.676767");'
);
$link->query('INSERT INTO t_test VALUES ();');

$stmt = $link->prepare('SELECT * FROM t_test;');
if ($stmt) {
    $stmt->execute();
    $tid = $t = $t2 = $t3 = $t4 = null;
    $stmt->bind_result($tid, $t, $t2, $t4, $t6);
    while ($stmt->fetch()) {
        var_dump($t, $t2, $t4, $t6);
    }
    $stmt->free_result();
} else {
    echo('[SECOND][FAIL] mysqli::prepare returned false: ' . $link->error . PHP_EOL);
}
$link->close();
?>
--EXPECT--
string(19) "2018-01-01 11:22:33"
string(22) "2018-02-02 11:22:33.77"
string(22) "2018-01-01 03:04:05.06"
string(24) "2018-03-03 11:22:33.8888"
string(24) "2018-01-01 03:04:05.0070"
string(26) "2018-04-04 11:22:33.999999"
string(26) "2018-01-01 03:04:05.008000"
string(8) "21:22:33"
string(11) "21:22:33.44"
string(13) "21:22:33.5555"
string(15) "21:22:33.676767"
string(8) "11:00:00"
string(11) "11:00:00.22"
string(13) "11:00:00.4444"
string(15) "11:00:00.006054"
--CLEAN--
<?php
require_once('connect.inc');
$link = new mysqli($host, $user, $passwd, $db, $port, $socket);
$link->query('DROP TABLE ts_test;');
$link->query('DROP TABLE t_test;');
$link->close();
?>
