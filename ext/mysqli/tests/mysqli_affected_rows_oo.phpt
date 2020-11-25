--TEST--
mysqli->affected_rows
--SKIPIF--
<?php
    require_once('skipif.inc');
    require_once('skipifemb.inc');
    require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $mysqli = new mysqli();
    if (false !== ($tmp = @$mysqli->affected_rows))
        printf("[000a] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }

    if (0 !== ($tmp = $mysqli->affected_rows))
    printf("[002] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query('DROP TABLE IF EXISTS test'))
        printf("[003] [%d] %s\n", $mysqli->errno, $mysqli->error);

    if (!$mysqli->query('CREATE TABLE test(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE = ' . $engine))
        printf("[004] [%d] %s\n", $mysqli->errno, $mysqli->error);

    if (!$mysqli->query("INSERT INTO test(id, label) VALUES (1, 'a')"))
        printf("[005] [%d] %s\n",  $mysqli->errno, $mysqli->error);

    if (1 !== ($tmp = $mysqli->affected_rows))
        printf("[006] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    // ignore INSERT error, NOTE: command line returns 0, affected_rows returns -1 as documented
    $mysqli->query("INSERT INTO test(id, label) VALUES (1, 'a')");
    if (-1 !== ($tmp = $mysqli->affected_rows))
        printf("[007] Expecting int/-1, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query("INSERT INTO test(id, label) VALUES (1, 'a') ON DUPLICATE KEY UPDATE id = 4"))
        printf("[008] [%d] %s\n",  $mysqli->errno, $mysqli->error);

    if (2 !== ($tmp = $mysqli->affected_rows))
        printf("[009] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query("INSERT INTO test(id, label) VALUES (2, 'b'), (3, 'c')"))
        printf("[010] [%d] %s\n",  $mysqli->errno, $mysqli->error);

    if (2 !== ($tmp = $mysqli->affected_rows))
        printf("[011] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query("INSERT IGNORE INTO test(id, label) VALUES (1, 'a')")) {
        printf("[012] [%d] %s\n",  $mysqli->errno, $mysqli->error);
    }

    if (1 !== ($tmp = $mysqli->affected_rows))
        printf("[013] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query("INSERT INTO test(id, label) SELECT id + 10, label FROM test"))
        printf("[014] [%d] %s\n",  $mysqli->errno, $mysqli->error);

    if (4 !== ($tmp = $mysqli->affected_rows))
        printf("[015] Expecting int/4, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query("REPLACE INTO test(id, label) values (4, 'd')"))
        printf("[015] [%d] %s\n",  $mysqli->errno, $mysqli->error);

    if (2 !== ($tmp = $mysqli->affected_rows))
        printf("[016] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query("REPLACE INTO test(id, label) values (5, 'e')"))
        printf("[017] [%d] %s\n",  $mysqli->errno, $mysqli->error);

    if (1 !== ($tmp = $mysqli->affected_rows))
        printf("[018] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query("UPDATE test SET label = 'a' WHERE id = 2"))
        printf("[019] [%d] %s\n",  $mysqli->errno, $mysqli->error);

    if (1 !== ($tmp = $mysqli->affected_rows))
        printf("[020] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query("UPDATE test SET label = 'a' WHERE id = 2")) {
        printf("[021] [%d] %s\n",  $mysqli->errno, $mysqli->error);
    }

    if (0 !== ($tmp = $mysqli->affected_rows))
        printf("[022] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query("UPDATE test SET label = 'a' WHERE id = 100")) {
        printf("[023] [%d] %s\n",  $mysqli->errno, $mysqli->error);
    }

    if (0 !== ($tmp = $mysqli->affected_rows))
        printf("[024] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (!$mysqli->query('DROP TABLE IF EXISTS test'))
        printf("[025] [%d] %s\n", $mysqli->errno, $mysqli->error);

    $mysqli->close();

    if (false !== ($tmp = @$mysqli->affected_rows))
        printf("[026] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!
