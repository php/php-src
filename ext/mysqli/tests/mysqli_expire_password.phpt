--TEST--
MySQL 5.6 / MariaDB 10.4.3 EXPIRE PASSWORD protocol change
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('connect.inc');

if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf("SKIP Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
        $host, $user, $db, $port, $socket));
}

if ($link->server_version < 50610)
    die(sprintf("SKIP Needs MySQL 5.6.10 or newer, found MySQL %s\n", $link->server_info));

if ($link->server_version >= 100000) {
    if ($link->server_version < 100403)
        die(sprintf("SKIP Needs MariaDB 10.4.3 or newer, found MariaDB %s\n", $link->server_info));
    $result = $link->query("select @@disconnect_on_expired_password");
    if (!$result)
        die("SKIP Failed to query MariaDB @@disconnect_on_expired_password value");
    $row = mysqli_fetch_row($result);
    if ($row[0] == 0)
        die("SKIP Cannot run in MariaDB @@disconnect_on_expired_password=OFF state");

}

if  (!$IS_MYSQLND && (mysqli_get_client_version() < 50610)) {
    die(sprintf("SKIP Needs libmysql 5.6.10 or newer, found  %s\n", mysqli_get_client_version()));
}

mysqli_query($link, 'DROP USER expiretest');
mysqli_query($link, 'DROP USER expiretest@localhost');

if (!mysqli_query($link, 'CREATE USER expiretest IDENTIFIED BY \'expiredpassword\'') ||
    !mysqli_query($link, 'CREATE USER expiretest@localhost IDENTIFIED BY \'expiredpassword\'')) {
    printf("skip Cannot create second DB user [%d] %s", mysqli_errno($link), mysqli_error($link));
    mysqli_close($link);
    die("skip CREATE USER failed");
}

if (!mysqli_query($link, 'ALTER USER expiretest PASSWORD EXPIRE') ||
    !mysqli_query($link, 'ALTER USER expiretest@localhost PASSWORD EXPIRE')) {
    printf("skip Cannot modify second DB user [%d] %s", mysqli_errno($link), mysqli_error($link));
    mysqli_close($link);
    die("skip ALTER USER failed");
}

if (!$link->query("DROP TABLE IF EXISTS test") ||
    !$link->query("CREATE TABLE test (id INT)") || !$link->query("INSERT INTO test(id) VALUES (1)"))
    die(sprintf("SKIP [%d] %s\n", $link->errno, $link->error));



if (!mysqli_query($link, sprintf("GRANT SELECT ON TABLE %s.test TO expiretest@'%%'", $db)) ||
    !mysqli_query($link, sprintf("GRANT SELECT ON TABLE %s.test TO expiretest@'localhost'", $db))) {
    printf("skip Cannot grant SELECT to user [%d] %s", mysqli_errno($link), mysqli_error($link));
    mysqli_close($link);
    die("skip GRANT failed");
}
?>
--FILE--
<?php
    require_once('connect.inc');
    require_once('table.inc');

    /* default */
    if (!$link = my_mysqli_connect($host, 'expiretest', 'expiredpassword', $db, $port, $socket)) {
        printf("[001] Cannot connect [%d] %s\n",
            mysqli_connect_errno(), mysqli_connect_error());
    } else {
        $link->query("SELECT id FROM test WHERE id = 1");
        printf("[002] Connect should fail, [%d] %s\n", $link->errno, $link->error);
    }

    /* explicitly requesting default */
    $link = mysqli_init();
    $link->options(MYSQLI_OPT_CAN_HANDLE_EXPIRED_PASSWORDS, 0);
    if (!my_mysqli_real_connect($link, $host, 'expiretest', 'expiredpassword', $db, $port, $socket)) {
        printf("[003] Cannot connect [%d] %s\n",
            mysqli_connect_errno(), mysqli_connect_error());
    } else {
        $link->query("SELECT id FROM test WHERE id = 1");
        printf("[004] Connect should fail, [%d] %s\n", $link->errno, $link->error);
    }

    /* allow connect */
    $link = mysqli_init();
    $link->options(MYSQLI_OPT_CAN_HANDLE_EXPIRED_PASSWORDS, 1);
    if (!my_mysqli_real_connect($link, $host, 'expiretest', 'expiredpassword', $db, $port, $socket)) {
        printf("[005] Cannot connect [%d] %s\n",
            mysqli_connect_errno(), mysqli_connect_error());
    } else {
        $link->query("SELECT id FROM test WHERE id = 1");
        printf("[006] Connect allowed, query fail, [%d] %s\n", $link->errno, $link->error);
        $link->close();
    }

    /* allow connect, fix pw */
    $link = mysqli_init();
    $link->options(MYSQLI_OPT_CAN_HANDLE_EXPIRED_PASSWORDS, 1);
    if (!my_mysqli_real_connect($link, $host, 'expiretest', 'expiredpassword', $db, $port, $socket)) {
        printf("[007] Cannot connect [%d] %s\n",
            mysqli_connect_errno(), mysqli_connect_error());
    } else {
        if (!$link->query("SET PASSWORD='expiretest'")) {
            $link->query("SET PASSWORD=PASSWORD('expiretest')");
        }
        printf("[008] Connect allowed, pw set, [%d] %s\n", $link->errno, $link->error);
        if ($res = $link->query("SELECT id FROM test WHERE id = 1"))
            var_dump($res->fetch_assoc());
        $link->close();
    }


    /* check login */
    if (!$link = my_mysqli_connect($host, 'expiretest', "expiretest", $db, $port, $socket)) {
        printf("[001] Cannot connect [%d] %s\n",
            mysqli_connect_errno(), mysqli_connect_error());
    } else {
        $link->query("SELECT id FROM test WHERE id = 1");
        if ($res = $link->query("SELECT id FROM test WHERE id = 1"))
            var_dump($res->fetch_assoc());
        $link->close();
    }



    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
    mysqli_query($link, 'DROP USER expiretest');
    mysqli_query($link, 'DROP USER expiretest@localhost');
?>
--EXPECTF--
Warning: mysqli%sconnect(): (HY000/%d): %s in %s on line %d
[001] Cannot connect [%d] %s

Warning: mysqli%sconnect(): (HY000/%d): %s in %s on line %d
[003] Cannot connect [%d] %s
[006] Connect allowed, query fail, [1820] %s
[008] Connect allowed, pw set, [0%A
array(1) {
  ["id"]=>
  string(1) "1"
}
array(1) {
  ["id"]=>
  string(1) "1"
}
done!
