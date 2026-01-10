--TEST--
mysqli_fetch_object with classes
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    class test_class {
        function __construct($arg1, $arg2) {
            echo __METHOD__ . "($arg1,$arg2)\n";
        }
    }

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    mysqli_select_db($link, $db);
    mysqli_query($link, "SET sql_mode=''");

    mysqli_query($link,"DROP TABLE IF EXISTS test_fetch");
    mysqli_query($link,"CREATE TABLE test_fetch(c1 smallint unsigned,
        c2 smallint unsigned,
        c3 smallint,
        c4 smallint,
        c5 smallint,
        c6 smallint unsigned,
        c7 smallint)");

    mysqli_query($link, "INSERT INTO test_fetch VALUES ( -23, 35999, NULL, -500, -9999999, -0, 0)");

    $result = mysqli_query($link, "SELECT * FROM test_fetch");
    $test = mysqli_fetch_object($result, 'test_class', array(1, 2));
    mysqli_free_result($result);

    var_dump($test);

    mysqli_close($link);

    echo "Done\n";
?>
--CLEAN--
<?php
require_once 'connect.inc';
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_fetch"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
test_class::__construct(1,2)
object(test_class)#%d (7) {
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
Done
