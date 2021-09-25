--TEST--
mysqli_stmt_result_metadata()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    require('table.inc');

    if (!$stmt = mysqli_stmt_init($link))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    try {
        mysqli_stmt_result_metadata($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test"))
        printf("[005] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!is_object(($res = mysqli_stmt_result_metadata($stmt))))
        printf("[006] Expecting object, got %s/%s\n", gettype($tmp), $tmp);

    if (2 !== ($tmp = mysqli_num_fields($res)))
        printf("[007] Expecting int/2, got %s/%s, [%d] %s\n",
            gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));

    if (!is_object($field0_fetch = mysqli_fetch_field($res)))
        printf("[008] Expecting object, got %s/%s, [%d] %s\n",
            gettype($field0_fetch), $field0_fetch, mysqli_errno($link), mysqli_error($link));

    if (!is_object($field0_direct = mysqli_fetch_field_direct($res, 0)))
        printf("[009] Expecting object, got %s/%s, [%d] %s\n",
            gettype($field0_direct), $field0_direct, mysqli_errno($link), mysqli_error($link));

    if ($field0_fetch != $field0_direct) {
        printf("[010] mysqli_fetch_field() differs from mysqli_fetch_field_direct()\n");
        var_dump($field0_fetch);
        var_dump($field0_direct);
    }

    var_dump($field0_fetch);

    if (!is_array($tmp = mysqli_fetch_fields($res)))
        printf("[011] Expecting array, got %s/%s, [%d] %s\n",
            gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));

    if (empty($tmp[0]) || empty($tmp[1]) || $tmp[0] != $field0_direct) {
        printf("[012] mysqli_fetch_fields() return value is suspicious\n");
        var_dump($tmp);
    }

    if (!mysqli_field_seek($res, 1))
        printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($field1_direct = mysqli_fetch_field_direct($res, 1)))
        printf("[014] Expecting object, got %s/%s, [%d] %s\n",
            gettype($field1_direct), $field1_direct, mysqli_errno($link), mysqli_error($link));

    if ($tmp[1] != $field1_direct) {
        printf("[015] mysqli_fetch_field_direct() differs from mysqli_fetch_fields()\n");
        var_dump($field1_direct);
        var_dump($tmp);
    }

    if (1 !== ($tmp = mysqli_field_tell($res)))
        printf("[016] Expecting int/1, got %s/%s, [%d] %s\n",
            gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));

    mysqli_free_result($res);
    mysqli_stmt_close($stmt);

    try {
        mysqli_stmt_result_metadata($stmt);
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
--EXPECTF--
mysqli_stmt object is not fully initialized
object(stdClass)#%d (13) {
  ["name"]=>
  string(2) "id"
  ["orgname"]=>
  string(2) "id"
  ["table"]=>
  string(4) "test"
  ["orgtable"]=>
  string(4) "test"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(0)
  ["length"]=>
  int(11)
  ["charsetnr"]=>
  int(63)
  ["flags"]=>
  int(49155)
  ["type"]=>
  int(3)
  ["decimals"]=>
  int(0)
}
mysqli_stmt object is already closed
done!
