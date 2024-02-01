--TEST--
mysqli thread_id & kill
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
    $version = $mysql->server_version;

    var_dump($mysql->ping());

    $ret = $mysql->kill($mysql->thread_id);
    if ($ret !== true){
        printf("[001] Expecting boolean/true got %s/%s\n", gettype($ret), var_export($ret, true));
    }

    var_dump($mysql->ping());

    $mysql->close();

    $mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    var_dump(mysqli_ping($mysql));

    $ret = $mysql->kill($mysql->thread_id);
    if ($ret !== true){
        printf("[002] Expecting boolean/true got %s/%s\n", gettype($ret), var_export($ret, true));
    }

    var_dump(mysqli_ping($mysql));

    $mysql->close();
    print "done!";
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
done!
