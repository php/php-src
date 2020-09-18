--TEST--
mysqli_fetch_field()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    // Note: no SQL type tests, internally the same function gets used as for mysqli_fetch_array() which does a lot of SQL type test

    require('table.inc');

    // Make sure that client, connection and result charsets are all the
    // same. Not sure whether this is strictly necessary.
    if (!mysqli_set_charset($link, 'utf8'))
        printf("[%d] %s\n", mysqli_errno($link), mysqli_errno($link));

    $charsetInfo = mysqli_get_charset($link);

    if (!$res = mysqli_query($link, "SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1")) {
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    /* ID column, binary charset */
    $tmp = mysqli_fetch_field($res);
    var_dump($tmp);

    /* label column, result set charset */
    $tmp = mysqli_fetch_field($res);
    var_dump($tmp);
    if ($tmp->charsetnr != $charsetInfo->number) {
        printf("[004] Expecting charset %s/%d got %d\n",
            $charsetInfo->charset, $charsetInfo->number, $tmp->charsetnr);
    }
    if ($tmp->length != $charsetInfo->max_length) {
        printf("[005] Expecting length %d got %d\n",
            $charsetInfo->max_length, $tmp->max_length);
    }
    if ($tmp->db != $db) {
        printf("011] Expecting database '%s' got '%s'\n",
            $db, $tmp->db);
    }

    var_dump(mysqli_fetch_field($res));

    mysqli_free_result($res);

    // Read http://bugs.php.net/bug.php?id=42344 on defaults!
    try {
        mysqli_fetch_field($res);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "CREATE TABLE test(id INT NOT NULL DEFAULT 1)"))
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test(id) VALUES (2)"))
        printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$res = mysqli_query($link, "SELECT id as _default_test FROM test")) {
        printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }
    var_dump(mysqli_fetch_assoc($res));
    /* binary */
    var_dump(mysqli_fetch_field($res));
    mysqli_free_result($res);

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
object(stdClass)#%d (13) {
  ["name"]=>
  string(2) "ID"
  ["orgname"]=>
  string(2) "id"
  ["table"]=>
  string(4) "TEST"
  ["orgtable"]=>
  string(%d) "%s"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(1)
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
object(stdClass)#%d (13) {
  ["name"]=>
  string(5) "label"
  ["orgname"]=>
  string(5) "label"
  ["table"]=>
  string(4) "TEST"
  ["orgtable"]=>
  string(%d) "%s"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(%d)
  ["length"]=>
  int(%d)
  ["charsetnr"]=>
  int(%d)
  ["flags"]=>
  int(0)
  ["type"]=>
  int(254)
  ["decimals"]=>
  int(0)
}
bool(false)
mysqli_result object is already closed
array(1) {
  ["_default_test"]=>
  string(1) "2"
}
object(stdClass)#%d (13) {
  ["name"]=>
  string(13) "_default_test"
  ["orgname"]=>
  string(2) "id"
  ["table"]=>
  string(%d) "%s"
  ["orgtable"]=>
  string(%d) "%s"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(1)
  ["length"]=>
  int(11)
  ["charsetnr"]=>
  int(63)
  ["flags"]=>
  int(32769)
  ["type"]=>
  int(3)
  ["decimals"]=>
  int(0)
}
done!
