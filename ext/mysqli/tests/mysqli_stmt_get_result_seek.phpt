--TEST--
mysqli_stmt_get_result() - seeking
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    if (!$stmt = mysqli_stmt_init($link))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id ASC LIMIT 3"))
        printf("[002] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!mysqli_stmt_execute($stmt))
        printf("[003] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (!is_object($res = mysqli_stmt_get_result($stmt)) || 'mysqli_result' != get_class($res)) {
        printf("[004] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
            gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    }

    if (3 !== $res->num_rows)
        printf("[005] Expecting 3 rows, got %s/%s rows\n", gettype($res->num_rows), $res->num_rows);

    if (2 !== $res->field_count)
        printf("[006] Expecting 2 fields, got %s/%s rows\n", gettype($res->field_count), $res->field_count);

    if (0 !== $res->current_field)
        printf("[006] Expecting offset 0, got %s/%s rows\n", gettype($res->current_field), $res->current_field);

    for ($i = 2; $i > 0; $i--) {
        if (!$res->data_seek($i))
            printf("[007] Cannot seek to position %d, [%d] %s\n",
                $i, mysqli_stmt_errno($stmt), $stmt->error);
        $row = $res->fetch_array(MYSQLI_BOTH);
        if (($row[0] !== $row['id']) || ($row[0] !== $i + 1)) {
            printf("[008] Record looks wrong, dumping data\n");
            var_dump($row);
        } else {
            unset($row[0]);
            unset($row['id']);
        }
        if ($row[1] !== $row['label']) {
            printf("[009] Record looks wrong, dumping data\n");
            var_dump($row);
        } else {
            unset($row[1]);
            unset($row['label']);
        }
        if (!empty($row)) {
            printf("[010] Not empty, dumping unexpected data\n");
            var_dump($row);
        }
    }

    try {
        $res->data_seek(-1);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    if (false !== ($tmp = $res->data_seek($res->num_rows + 1)))
        printf("[012] Expecting boolean/false got %s/%s\n", gettype($tmp), $tmp);

    for ($i = 0; $i < 100; $i++) {
        /* intentionally out of range! */
        $pos = mt_rand(-1, 4);
        try {
            $tmp = @mysqli_data_seek($res, $pos);
            if (($pos >= 0 && $pos < 3)) {
                if (true !== $tmp)
                    printf("[015] Expecting boolan/true got %s/%s\n", gettype($tmp), $tmp);
                $row = $res->fetch_array(MYSQLI_NUM);
                if ($row[0] !== $pos + 1)
                    printf("[016] Expecting id = %d for pos %d got %s/%s\n",
                        $pos + 1, $pos, gettype($row[0]), $row[0]);
            } else {
                if (false !== $tmp)
                    printf("[014] Expecting boolan/false got %s/%s\n", gettype($tmp), $tmp);
            }
        } catch (\ValueError $e) { /* Suppress because RANDOM */}
    }

    mysqli_stmt_close($stmt);

    if (true !== ($tmp = mysqli_data_seek($res, 0)))
        printf("[015] Expecting boolan/true got %s/%s\n", gettype($tmp), $tmp);

    if (!is_array($row = $res->fetch_array(MYSQLI_NUM)))
        printf("[016] Expecting array got %s/%s\n", gettype($tmp), $tmp);

    mysqli_free_result($res);

    try {
        mysqli_data_seek($res, 0);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    try {
        $res->fetch_array(MYSQLI_NUM);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    mysqli_close($link);

    try {
        mysqli_data_seek($res, 0);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    try {
        $res->fetch_array(MYSQLI_NUM);
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
mysqli_result::data_seek(): Argument #1 ($offset) must be greater than or equal to 0
mysqli_result object is already closed
mysqli_result object is already closed
mysqli_result object is already closed
mysqli_result object is already closed
done!
