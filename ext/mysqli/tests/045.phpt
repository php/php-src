--TEST--
mysqli_stmt_bind_result (SHOW)
--SKIPIF--
<?php
    require_once('skipif.inc');
    require_once('skipifemb.inc');
    require_once('skipifconnectfailure.inc');

    require_once("connect.inc");
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    $stmt = mysqli_prepare($link, "SHOW VARIABLES LIKE 'port'");
    mysqli_stmt_execute($stmt);

    if (!$stmt->field_count) {
        printf("skip SHOW command is not supported in prepared statements.");
    }
    $stmt->close();
    mysqli_close($link);
?>
--FILE--
<?php
    require_once("connect.inc");

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    $stmt = mysqli_prepare($link, "SHOW VARIABLES LIKE 'port'");
    mysqli_stmt_execute($stmt);

    mysqli_stmt_bind_result($stmt, $c1, $c2);
    mysqli_stmt_fetch($stmt);
    mysqli_stmt_close($stmt);
    $test = array ($c1,$c2);

    var_dump($test);

    mysqli_close($link);
    print "done!";
?>
--EXPECTF--
array(2) {
  [0]=>
  string(4) "port"
  [1]=>
  string(%d) "%s"
}
done!
