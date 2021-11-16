--TEST--
mysqli_num_fields()
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

    function func_test_mysqli_num_fields($link, $query, $expected, $offset, $test_free = false) {

        if (!($res = mysqli_query($link, $query))) {
            printf("[%03d] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
            return;
        }

        if ($expected !== ($tmp = mysqli_num_fields($res)))
            printf("[%03d] Expecting %s/%d, got %s/%d\n", $offset + 1,
                gettype($expected), $expected,
                gettype($tmp), $tmp);

        mysqli_free_result($res);

        try {
            mysqli_num_fields($res);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }
    }

    func_test_mysqli_num_fields($link, "SELECT 1 AS a", 1, 5);
    func_test_mysqli_num_fields($link, "SELECT id, label FROM test", 2, 10);
    func_test_mysqli_num_fields($link, "SELECT 1 AS a, NULL AS b, 'foo' AS c", 3, 15);
    func_test_mysqli_num_fields($link, "SELECT id FROM test", 1, 20, true);

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
mysqli_result object is already closed
mysqli_result object is already closed
mysqli_result object is already closed
mysqli_result object is already closed
done!
