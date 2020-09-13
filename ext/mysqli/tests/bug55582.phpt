--TEST--
Bug #55582 mysqli_num_rows() returns always 0 for unbuffered, when mysqlnd is used
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once("connect.inc");
?>
--FILE--
<?php
    include "connect.inc";
    if (!($link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))) {
        printf("[001] Cannot connect to the server");
    }

    var_dump($link->real_query("SELECT 1"));
    $res = $link->use_result();
    try {
        mysqli_num_rows($res);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
    var_dump($res->fetch_assoc());
    try {
        mysqli_num_rows($res);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
    var_dump($res->fetch_assoc());
    var_dump(mysqli_num_rows($res));

    $link->close();
    echo "done\n";
?>
--EXPECTF--
bool(true)
Function cannot be used in MYSQLI_USE_RESULT mode
array(1) {
  [1]=>
  string(1) "1"
}
Function cannot be used in MYSQLI_USE_RESULT mode
NULL
int(1)
done
