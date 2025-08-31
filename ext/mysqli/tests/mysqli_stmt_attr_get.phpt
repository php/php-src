--TEST--
mysqli_stmt_attr_get()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    $valid_attr = array(
        MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH,
        MYSQLI_STMT_ATTR_CURSOR_TYPE,
    );

    $stmt = mysqli_stmt_init($link);
    mysqli_stmt_prepare($stmt, 'SELECT * FROM test');

    try {
        mysqli_stmt_attr_get($stmt, -100);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    foreach ($valid_attr as $attr) {
        try {
            mysqli_stmt_attr_get($stmt, $attr);
        } catch (Throwable $exception) {
            echo $exception->getMessage() . "\n";
        }
    }

    $stmt->close();

    foreach ($valid_attr as $attr) {
        try {
            mysqli_stmt_attr_get($stmt, $attr);
        } catch (Throwable $exception) {
            echo $exception->getMessage() . "\n";
        }
    }

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECT--
mysqli_stmt_attr_get(): Argument #2 ($attribute) must be either MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH or MYSQLI_STMT_ATTR_CURSOR_TYPE
mysqli_stmt object is already closed
mysqli_stmt object is already closed
done!
