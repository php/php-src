--TEST--
mysqli_fetch_field_direct()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    require('table.inc');

    if (!$res = mysqli_query($link, "SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1")) {
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    try {
        var_dump(mysqli_fetch_field_direct($res, -1));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    var_dump(mysqli_fetch_field_direct($res, 0));

    try {
        var_dump(mysqli_fetch_field_direct($res, 2));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    mysqli_free_result($res);

    try {
        mysqli_fetch_field_direct($res, 0);
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
mysqli_fetch_field_direct(): Argument #2 ($index) must be greater than or equal to 0
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
  int(%d)
  ["length"]=>
  int(%d)
  ["charsetnr"]=>
  int(%d)
  ["flags"]=>
  int(%d)
  ["type"]=>
  int(%d)
  ["decimals"]=>
  int(%d)
}
mysqli_fetch_field_direct(): Argument #2 ($index) must be less than the number of fields for this result set
mysqli_result object is already closed
done!
