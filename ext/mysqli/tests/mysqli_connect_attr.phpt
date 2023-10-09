--TEST--
mysqli check the session_connect_attrs table for connection attributes
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';

if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

/* skip test if the server version does not have session_connect_attrs table yet*/
if (!$res = mysqli_query($link, "select count(*) as count from information_schema.tables where table_schema='performance_schema' and table_name='session_connect_attrs';"))
    die("skip select from information_schema.tables for session_connect_attrs  query failed");

$tmp = mysqli_fetch_assoc($res);
mysqli_free_result($res);
if($tmp['count'] == "0") {
    mysqli_close($link);
    die("skip mysql does not support session_connect_attrs table yet");
}

/* skip test if performance_schema is OFF*/
if (!$res = mysqli_query($link, "show variables like 'performance_schema';"))
    die("skip show variables like 'performance_schema' failed");

$tmp = mysqli_fetch_assoc($res);
mysqli_free_result($res);
if($tmp['Value'] == "OFF") {
    mysqli_close($link);
    die("skip performance_schema is OFF");
}

mysqli_close($link);
?>
--FILE--
<?php
    require_once 'connect.inc';

    $tmp    = NULL;
    $link   = NULL;
    $res    = NULL;
    if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",$host, $user, $db, $port, $socket);

    //in case $host is empty, do not test for _server_host field
    if (isset($host) && trim($host) != '') {
        if (!$res = mysqli_query($link, "select * from performance_schema.session_connect_attrs where ATTR_NAME='_server_host' and processlist_id = connection_id()")) {
            printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
        } else {
            $tmp = mysqli_fetch_assoc($res);
            if (!$tmp || !isset($tmp['ATTR_NAME'])) {
                echo "[003] _server_host missing\n";
            } elseif ($tmp['ATTR_VALUE'] !== $host) {
                printf("[004] _server_host value mismatch\n") ;
            }
            mysqli_free_result($res);
        }
    }

    if (!$res = mysqli_query($link, "select * from performance_schema.session_connect_attrs where ATTR_NAME='_client_name' and processlist_id = connection_id()")) {
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    } else {
        $tmp = mysqli_fetch_assoc($res);
        if (!$tmp || !isset($tmp['ATTR_NAME'])) {
            echo "[006] _client_name missing\n";
        } elseif ($tmp['ATTR_VALUE'] !== "mysqlnd") {
            printf("[007] _client_name value mismatch\n") ;
        }
        mysqli_free_result($res);
    }

    printf("done!");
?>
--EXPECT--
done!
