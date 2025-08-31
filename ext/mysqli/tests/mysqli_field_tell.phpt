--TEST--
mysqli_field_tell()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';
    if (!$res = mysqli_query($link, "SELECT id FROM test ORDER BY id LIMIT 1", MYSQLI_USE_RESULT)) {
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    var_dump(mysqli_field_tell($res));
    var_dump(mysqli_field_tell($res));
    var_dump(mysqli_fetch_field($res));
    var_dump(mysqli_fetch_field($res));
    var_dump(mysqli_field_tell($res));

    try {
        var_dump(mysqli_field_seek($res, 2));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    var_dump(mysqli_field_tell($res));

    try {
        var_dump(mysqli_field_seek($res, -1));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    var_dump(mysqli_field_tell($res));

    var_dump(mysqli_field_seek($res, 0));
    var_dump(mysqli_field_tell($res));



    mysqli_free_result($res);

    try {
        mysqli_field_tell($res);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECTF--
int(0)
int(0)
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
bool(false)
int(1)
mysqli_field_seek(): Argument #2 ($index) must be less than the number of fields for this result set
int(1)
mysqli_field_seek(): Argument #2 ($index) must be greater than or equal to 0
int(1)
bool(true)
int(0)
mysqli_result object is already closed
done!
