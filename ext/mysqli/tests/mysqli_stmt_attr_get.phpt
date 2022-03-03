--TEST--
mysqli_stmt_attr_get()
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

    $valid_attr = array(
        "max_length" => MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH,
        "cursor_type" => MYSQLI_STMT_ATTR_CURSOR_TYPE,
        "prefetch_rows" => MYSQLI_STMT_ATTR_PREFETCH_ROWS,
    );

    $stmt = mysqli_stmt_init($link);
    mysqli_stmt_prepare($stmt, 'SELECT * FROM test');

    try {
        mysqli_stmt_attr_get($stmt, -100);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    foreach ($valid_attr as $k => $attr) {
        /* This can't happen anymore as it only returns int */
        if (false === ($tmp = mysqli_stmt_attr_get($stmt, $attr))) {
            printf("[006] Expecting any type, but not boolean/false, got %s/%s for attribute %s/%s\n",
                gettype($tmp), $tmp, $k, $attr);
        }
    }

    $stmt->close();

    foreach ($valid_attr as $k => $attr) {
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
    require_once("clean_table.inc");
?>
--EXPECT--
mysqli_stmt_attr_get(): Argument #2 ($attribute) must be one of MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH, MYSQLI_STMT_ATTR_PREFETCH_ROWS, or STMT_ATTR_CURSOR_TYPE
mysqli_stmt object is already closed
mysqli_stmt object is already closed
mysqli_stmt object is already closed
done!
