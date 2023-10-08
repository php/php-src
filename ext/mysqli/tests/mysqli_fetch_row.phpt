--TEST--
mysqli_fetch_row()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';
    if (!$res = mysqli_query($link, "SELECT id, label, id AS _id FROM test ORDER BY id LIMIT 1")) {
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    print "[004]\n";
    var_dump(mysqli_fetch_row($res));

    print "[005]\n";
    var_dump(mysqli_fetch_row($res));

    mysqli_free_result($res);

    try {
        mysqli_fetch_row($res);
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
--EXPECT--
[004]
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "a"
  [2]=>
  string(1) "1"
}
[005]
NULL
mysqli_result object is already closed
done!
