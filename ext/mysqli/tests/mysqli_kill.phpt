--TEST--
mysqli_kill()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    try {
        mysqli_kill($link, 0);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    if (!$thread_id = mysqli_thread_id($link))
        printf("[004] Cannot determine thread id, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $tmp = mysqli_kill($link, $thread_id);
    if (!is_bool($tmp))
        printf("[005] Expecting boolean/any, got %s/%s\n", gettype($tmp), $tmp);

    if ($res = mysqli_query($link, "SELECT id FROM test LIMIT 1"))
        printf("[006] Expecting boolean/false, got %s/%s\n", gettype($res), $res);

    var_dump($error = mysqli_error($link));
    if (!is_string($error) || ('' === $error))
        printf("[007] Expecting string/any non empty, got %s/%s\n", gettype($error), $error);
    var_dump($res);
    var_dump($link);
    if ($link->info != 'Records: 6  Duplicates: 0  Warnings: 0') {
        printf("[008] mysqlnd used to be more verbose and used to support SELECT\n");
    }

    mysqli_close($link);

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[010] Cannot connect, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

    try {
        mysqli_kill($link, -1);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    if ((!$res = mysqli_query($link, "SELECT id FROM test LIMIT 1")) ||
        (!$tmp = mysqli_fetch_assoc($res))) {
        printf("[011] Connection should not be gone, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }
    var_dump($tmp);
    mysqli_free_result($res);
    mysqli_close($link);

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[012] Cannot connect, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

    mysqli_change_user($link, "This might work if you accept anonymous users in your setup", "password", $db);
    try {
        mysqli_kill($link, -1);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECTF--
mysqli_kill(): Argument #2 ($process_id) must be greater than 0
string(%d) "%s"
bool(false)
object(mysqli)#%d (%d) {
  ["affected_rows"]=>
  int(-1)
  ["client_info"]=>
  string(%d) "%s"
  ["client_version"]=>
  int(%d)
  ["connect_errno"]=>
  int(0)
  ["connect_error"]=>
  NULL
  ["errno"]=>
  int(%d)
  ["error"]=>
  string(%d) "%s"
  ["error_list"]=>
  array(1) {
    [0]=>
    array(3) {
      ["errno"]=>
      int(%d)
      ["sqlstate"]=>
      string(5) "%s"
      ["error"]=>
      string(%d) "%s"
    }
  }
  ["field_count"]=>
  int(1)
  ["host_info"]=>
  string(%d) "%s"
  ["info"]=>
  NULL
  ["insert_id"]=>
  int(0)
  ["server_info"]=>
  string(%d) "%s"
  ["server_version"]=>
  int(%d)
  ["sqlstate"]=>
  string(%d) "%s"
  ["protocol_version"]=>
  int(%d)
  ["thread_id"]=>
  int(%d)
  ["warning_count"]=>
  int(%d)
}
mysqli_kill(): Argument #2 ($process_id) must be greater than 0
array(1) {
  ["id"]=>
  string(1) "1"
}
mysqli_kill(): Argument #2 ($process_id) must be greater than 0
done!
