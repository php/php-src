--TEST--
mysqli_commit()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

require_once('connect.inc');
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    die(sprintf("skip Cannot connect, [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

if (!have_innodb($link))
    die(sprintf("skip Needs InnoDB support, [%d] %s", $link->errno, $link->error));
?>
--FILE--
<?php
    require_once("connect.inc");

    $tmp    = NULL;
    $link   = NULL;

    $mysqli = new mysqli();
    if (false !== ($tmp = @$mysqli->commit())) {
        printf("[013] Expecting false got %s/%s\n", gettype($tmp), $tmp);
    }

    if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }

    if (true !== ($tmp = $mysqli->commit())) {
        printf("[002] Expecting boolean/true got %s/%s\n", gettype($tmp), $tmp);
    }

    if (true !== ($tmp = $mysqli->autocommit(false))) {
        printf("[003] Cannot turn off autocommit, expecting true, got %s/%s\n", gettype($tmp), $tmp);
    }

    if (!$mysqli->query('DROP TABLE IF EXISTS test')) {
        printf("[004] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    if (!$mysqli->query('CREATE TABLE test(id INT) ENGINE = InnoDB')) {
        printf("[005] Cannot create test table, [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    if (!$mysqli->query('INSERT INTO test(id) VALUES (1)')) {
        printf("[006] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    $tmp = $mysqli->commit();
    if ($tmp !== true) {
        printf("[007] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
    }

    if (!$mysqli->query('ROLLBACK'))
        printf("[008] [%d] %s\n", $mysqli->errno, $mysqli->error);

    if (!$res = $mysqli->query('SELECT COUNT(*) AS num FROM test')) {
        printf("[009] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    $tmp = $res->fetch_assoc();
    if (1 != $tmp['num']) {
        printf("[010] Expecting 1 row in table test, found %d rows\n", $tmp['num']);
    }
    $res->free();

    if (!$mysqli->query('DROP TABLE IF EXISTS test')) {
        printf("[011] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    if (!$mysqli->commit(0 , "tx_name0123")) {
        printf("[012] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }
    if (!$mysqli->commit(0 , "*/ nonsense")) {
        printf("[013] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }
    if (!$mysqli->commit(0 , "tx_name ulf вендел")) {
        printf("[014] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }
    if (!$mysqli->commit(0 , "tx_name \t\n\r\b")) {
        printf("[015] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }
    if (!$mysqli->commit(MYSQLI_TRANS_COR_AND_CHAIN | MYSQLI_TRANS_COR_NO_RELEASE , "tx_name")) {
        printf("[016] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    $mysqli->close();

    if (false !== ($tmp = @$mysqli->commit())) {
        printf("[017] Expecting false, got %s/%s\n", gettype($tmp), $tmp);
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli::commit(): Transaction name truncated. Must be only [0-9A-Za-z\-_=]+ in %s on line %d

Warning: mysqli::commit(): Transaction name truncated. Must be only [0-9A-Za-z\-_=]+ in %s on line %d

Warning: mysqli::commit(): Transaction name truncated. Must be only [0-9A-Za-z\-_=]+ in %s on line %d
done!
