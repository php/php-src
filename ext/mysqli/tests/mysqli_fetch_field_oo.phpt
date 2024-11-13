--TEST--
mysqli_fetch_field()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    // Note: no SQL type tests, internally the same function gets used as for mysqli_fetch_array() which does a lot of SQL type test
    require 'table.inc';
    $mysqli = $link;

    // Make sure that client, connection and result charsets are all the
    // same. Not sure whether this is strictly necessary.
    if (!$mysqli->set_charset('utf8'))
        printf("[%d] %s\n", $mysqli->errno, $mysqli->errno);

    $charsetInfo = $mysqli->get_charset();

    if (!$res = $mysqli->query("SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1")) {
        printf("[004] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    var_dump($res->fetch_field());

    $tmp = $res->fetch_field();
    var_dump($tmp);
    if ($tmp->charsetnr != $charsetInfo->number) {
        printf("[005] Expecting charset %s/%d got %d\n",
            $charsetInfo->charset, $charsetInfo->number, $tmp->charsetnr);
    }
    if ($tmp->db != $db) {
        printf("[007] Expecting database '%s' got '%s'\n",
          $db, $tmp->db);
    }

    var_dump($res->fetch_field());

    $res->free_result();

    try {
        $res->fetch_field();
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    $mysqli->close();
    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
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
object(stdClass)#%d (13) {
  ["name"]=>
  string(5) "label"
  ["orgname"]=>
  string(5) "label"
  ["table"]=>
  string(4) "TEST"
  ["orgtable"]=>
  string(4) "test"
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
done!
