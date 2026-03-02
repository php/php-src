--TEST--
mysqli_free_result()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';
    if (!$res = mysqli_query($link, "SELECT id FROM test ORDER BY id LIMIT 1")) {
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    print "a\n";
    var_dump(mysqli_free_result($res));

    print "b\n";
    try {
        mysqli_free_result($res);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!$res = mysqli_query($link, "SELECT id FROM test ORDER BY id LIMIT 1")) {
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    print "c\n";
    var_dump(mysqli_store_result($link));
    var_dump(mysqli_error($link));
    print "[005]\n";

    mysqli_free_result($res);

    if (!$res = mysqli_query($link, "SELECT id FROM test ORDER BY id LIMIT 1")) {
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }
    print "d\n";
    var_dump(mysqli_use_result($link));
    var_dump(mysqli_error($link));
    print "[007]\n";
    var_dump(mysqli_free_result($res));

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECT--
a
NULL
b
mysqli_result object is already closed
c
bool(false)
string(0) ""
[005]
d
bool(false)
string(0) ""
[007]
NULL
done!
