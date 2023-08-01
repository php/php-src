--TEST--
$res->fetch_field_direct(s)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';
    $mysqli = $link;

    if (!$res = $mysqli->query("SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1")) {
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    try {
        var_dump($res->fetch_field_direct(-1));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    var_dump($res->fetch_field_direct(0));

    try {
        var_dump($res->fetch_field_direct(2));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    $res->free_result();

    try {
        $res->fetch_field_direct(0);
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
mysqli_result::fetch_field_direct(): Argument #1 ($index) must be greater than or equal to 0
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
  int(%d)
  ["length"]=>
  int(11)
  ["charsetnr"]=>
  int(%d)
  ["flags"]=>
  int(%d)
  ["type"]=>
  int(%d)
  ["decimals"]=>
  int(%d)
}
mysqli_result::fetch_field_direct(): Argument #1 ($index) must be less than the number of fields for this result set
mysqli_result object is already closed
done!
