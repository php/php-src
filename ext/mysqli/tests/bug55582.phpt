--TEST--
Bug #55582 mysqli_num_rows() returns always 0 for unbuffered, when mysqlnd is used
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
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
        var_dump(mysqli_num_rows($res));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    var_dump($res->fetch_assoc());
    try {
        var_dump(mysqli_num_rows($res));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    var_dump($res->fetch_assoc());
    var_dump(mysqli_num_rows($res));

    $link->close();
    echo "done\n";
?>
--EXPECT--
bool(true)
mysqli_num_rows() cannot be used in MYSQLI_USE_RESULT mode
array(1) {
  [1]=>
  string(1) "1"
}
mysqli_num_rows() cannot be used in MYSQLI_USE_RESULT mode
NULL
int(1)
done
