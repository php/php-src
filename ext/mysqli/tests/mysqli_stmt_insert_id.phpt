--TEST--
mysqli_stmt_insert_id()
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

    $stmt = mysqli_stmt_init($link);

    try {
        mysqli_stmt_insert_id($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 1") ||
        !mysqli_stmt_execute($stmt)) {
        printf("[004] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    }

    if (0 !== ($tmp = mysqli_stmt_insert_id($stmt)))
        printf("[005] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);
    mysqli_stmt_close($stmt);

    // no auto_increment column
    $stmt = mysqli_stmt_init($link);
    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (100, 'a')") ||
        !mysqli_stmt_execute($stmt)) {
        printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    }

    if (0 !== ($tmp = mysqli_stmt_insert_id($stmt)))
        printf("[007] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (mysqli_get_server_version($link) > 50000 &&
        (!mysqli_stmt_prepare($stmt, "ALTER TABLE test MODIFY id INT NOT NULL AUTO_INCREMENT") ||
        !mysqli_stmt_execute($stmt))) {
        printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    } else if (mysqli_get_server_version($link) < 50000){
        mysqli_query($link, "ALTER TABLE test MODIFY id INT NOT NULL AUTO_INCREMENT");
    }

    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(label) VALUES ('a')") ||
        !mysqli_stmt_execute($stmt)) {
        printf("[009] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    }
    if (0 === ($tmp = mysqli_stmt_insert_id($stmt)))
        printf("[010] Expecting int/any non zero, got %s/%s\n", gettype($tmp), $tmp);
    mysqli_stmt_close($stmt);

    mysqli_close($link);

    try {
        mysqli_stmt_insert_id($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
mysqli_stmt object is not fully initialized
mysqli_stmt object is already closed
done!
