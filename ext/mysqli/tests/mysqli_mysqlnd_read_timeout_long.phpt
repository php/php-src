--TEST--
mysqlnd.net_read_timeout > default_socket_timeout
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');

if (!$IS_MYSQLND) {
    die("skip: test applies only to mysqlnd");
}

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf('skip Cannot connect to MySQL, [%d] %s.', mysqli_connect_errno(), mysqli_connect_error()));
}
if (mysqli_get_server_version($link) <= 50011) {
    die(sprintf('skip Needs MySQL 5.0.12+, found version %d.', mysqli_get_server_version($link)));
}
?>
--INI--
default_socket_timeout=1
mysqlnd.net_read_timeout=12
max_execution_time=12
--FILE--
<?php
    set_time_limit(12);
    include ("connect.inc");

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }

    if (!$res = mysqli_query($link, "SELECT SLEEP(6)"))
        printf("[002] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

    var_dump($res->fetch_assoc());

    mysqli_free_result($res);
    mysqli_close($link);

    print "done!";
?>
--EXPECT--
array(1) {
  ["SLEEP(6)"]=>
  string(1) "0"
}
done!
