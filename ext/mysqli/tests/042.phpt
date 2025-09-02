--TEST--
mysqli_fetch_object
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    mysqli_select_db($link, $db);
    mysqli_query($link, "SET sql_mode=''");

    mysqli_query($link,"DROP TABLE IF EXISTS test_bind_fetch");
    mysqli_query($link,"CREATE TABLE test_bind_fetch(c1 smallint unsigned,
        c2 smallint unsigned,
        c3 smallint,
        c4 smallint,
        c5 smallint,
        c6 smallint unsigned,
        c7 smallint) ENGINE=" . $engine);

    $stmt = mysqli_prepare($link, "INSERT INTO test_bind_fetch VALUES (?,?,?,?,?,?,?)");
    mysqli_stmt_bind_param($stmt, "iiiiiii", $c1,$c2,$c3,$c4,$c5,$c6,$c7);

    $c1 = -23;
    $c2 = 35999;
    $c3 = NULL;
    $c4 = -500;
    $c5 = -9999999;
    $c6 = -0;
    $c7 = 0;

    mysqli_stmt_execute($stmt);
    mysqli_stmt_close($stmt);

    $result = mysqli_query($link, "SELECT * FROM test_bind_fetch");
    $test = mysqli_fetch_object($result);
    mysqli_free_result($result);

    var_dump($test);

    mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch");
    mysqli_close($link);
    print "done!"
?>
--CLEAN--
<?php
require_once 'connect.inc';
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
object(stdClass)#%d (7) {
  ["c1"]=>
  string(1) "0"
  ["c2"]=>
  string(5) "35999"
  ["c3"]=>
  NULL
  ["c4"]=>
  string(4) "-500"
  ["c5"]=>
  string(6) "-32768"
  ["c6"]=>
  string(1) "0"
  ["c7"]=>
  string(1) "0"
}
done!
