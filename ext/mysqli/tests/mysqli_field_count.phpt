--TEST--
mysqli_field_count()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    var_dump(mysqli_field_count($link));

    if (!$res = mysqli_query($link, "SELECT * FROM test ORDER BY id LIMIT 1")) {
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    var_dump(mysqli_field_count($link));

    mysqli_free_result($res);

    if (!mysqli_query($link, "INSERT INTO test(id, label) VALUES (100, 'x')"))
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    var_dump($link->field_count);
    var_dump(mysqli_field_count($link));

    if (!$res = mysqli_query($link, "SELECT NULL as _null, '' AS '', 'three' AS 'drei'"))
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    var_dump(mysqli_field_count($link));
    mysqli_free_result($res);

    mysqli_close($link);

    try {
        mysqli_field_count($link);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECT--
int(0)
int(2)
int(0)
int(0)
int(3)
mysqli object is already closed
done!
