--TEST--
mysqli_stmt_result_metadata
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    mysqli_select_db($link, $db);

    mysqli_query($link,"DROP TABLE IF EXISTS test_store_result");
    mysqli_query($link,"CREATE TABLE test_store_result (a int)");

    mysqli_query($link, "INSERT INTO test_store_result VALUES (1),(2),(3)");

    $stmt = mysqli_prepare($link, "SELECT * FROM test_store_result");
    mysqli_stmt_execute($stmt);

    /* this should produce an out of sync error */
    if ($result = mysqli_query($link, "SELECT * FROM test_store_result")) {
        mysqli_free_result($result);
        printf ("Query ok\n");
    }
    mysqli_stmt_close($stmt);

    /* now we should try mysqli_stmt_reset() */
    $stmt = mysqli_prepare($link, "SELECT * FROM test_store_result");
    var_dump(mysqli_stmt_execute($stmt));
    var_dump(mysqli_stmt_reset($stmt));

    var_dump($stmt = mysqli_prepare($link, "SELECT * FROM test_store_result"));
    if ($stmt->affected_rows !== 0)
            printf("[001] Expecting 0, got %d\n", $stmt->affected_rows);

    var_dump(mysqli_stmt_execute($stmt));
    var_dump($stmt = @mysqli_prepare($link, "SELECT * FROM test_store_result"), mysqli_error($link));
    var_dump(mysqli_stmt_reset($stmt));

    $stmt = mysqli_prepare($link, "SELECT * FROM test_store_result");
    mysqli_stmt_execute($stmt);
    $result1 = mysqli_stmt_result_metadata($stmt);
    mysqli_stmt_store_result($stmt);

    printf ("Rows: %d\n", mysqli_stmt_affected_rows($stmt));

    /* this should show an error, cause results are not buffered */
    if ($result = mysqli_query($link, "SELECT * FROM test_store_result")) {
        $row = mysqli_fetch_row($result);
        mysqli_free_result($result);
    }

    var_dump($row);

    mysqli_free_result($result1);
    mysqli_stmt_close($stmt);
    mysqli_close($link);
    echo "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_store_result"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
bool(true)
bool(true)
object(mysqli_stmt)#%d (%d) {
  ["affected_rows"]=>
  int(%i)
  ["insert_id"]=>
  int(0)
  ["num_rows"]=>
  int(0)
  ["param_count"]=>
  int(0)
  ["field_count"]=>
  int(1)
  ["errno"]=>
  int(0)
  ["error"]=>
  string(0) ""
  ["error_list"]=>
  array(0) {
  }
  ["sqlstate"]=>
  string(5) "00000"
  ["id"]=>
  int(%d)
}
bool(true)
bool(false)
string(0) ""

Warning: mysqli_stmt_reset() expects parameter 1 to be mysqli_stmt, bool given in %s on line %d
NULL
Rows: 3
array(1) {
  [0]=>
  string(1) "1"
}
done!
