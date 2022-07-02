--TEST--
mysqli_get_client_stats() - skipped rows
--INI--
mysqlnd.collect_statistics="1"
mysqlnd.collect_memory_statistics="1"
--EXTENSIONS--
mysqli
--SKIPIF--
<?PHP
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_get_client_stats')) {
    die("skip only available with mysqlnd");
}
?>
--FILE--
<?php
    require_once('connect.inc');
    require_once('table.inc');

    if (!$res = mysqli_query($link, 'SELECT id FROM test', MYSQLI_STORE_RESULT))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $num_rows = mysqli_num_rows($res);
    assert($num_rows > 2);
    mysqli_free_result($res);

    $before = mysqli_get_client_stats();
    printf("BEFORE: rows_skipped_normal = %d\n", $before['rows_skipped_normal']);

    if (!$res = mysqli_query($link, 'SELECT id FROM test', MYSQLI_USE_RESULT))
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    /* fetch all rows but the last one */
    for ($i = 0; $i < $num_rows - 1; $i++)
        $row = mysqli_fetch_assoc($res);

    /* enforce implicit cleaning of the wire and skipping the last row */
    mysqli_free_result($res);
    $after = mysqli_get_client_stats();
    printf("AFTER: rows_skipped_normal = %d\n", $after['rows_skipped_normal']);

    if ($after['rows_skipped_normal'] != $before['rows_skipped_normal'] + 1)
        printf("Statistics should show an increase of 1 for rows_skipped_normal, ".
                "but before=%d after=%d\n", $before['rows_skipped_normal'], $after['rows_skipped_normal']);

    mysqli_close($link);
    print "done!";
?>
--EXPECTF--
BEFORE: rows_skipped_normal = %d
AFTER: rows_skipped_normal = %d
done!
