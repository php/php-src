--TEST--
mysqli_fetch_lengths()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!$mysqli = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }

    require('table.inc');
    if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id LIMIT 1")) {
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    var_dump(mysqli_fetch_lengths($res));
    while ($row = mysqli_fetch_assoc($res))
        var_dump(mysqli_fetch_lengths($res));
    var_dump(mysqli_fetch_lengths($res));

    mysqli_free_result($res);

    try {
        mysqli_fetch_lengths($res);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}
bool(false)
mysqli_result object is already closed
done!
