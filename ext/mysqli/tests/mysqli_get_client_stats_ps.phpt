--TEST--
mysqli_get_client_stats() - PS
--SKIPIF--
<?PHP
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_get_client_stats')) {
    die("skip only available with mysqlnd");
}
?>
--INI--
mysqlnd.collect_statistics=1
mysqlnd.collect_memory_statistics=1
--FILE--
<?php
    require_once('connect.inc');
    require_once('table.inc');

    $stats = mysqli_get_client_stats();
    printf("BEGINNING: rows_fetched_from_client_ps_unbuffered = %d\n",	$stats['rows_fetched_from_client_ps_unbuffered']);
    printf("BEGINNING: rows_fetched_from_client_ps_buffered = %d\n",	$stats['rows_fetched_from_client_ps_buffered']);
    printf("BEGINNING: rows_fetched_from_client_ps_cursor = %d\n",	$stats['rows_fetched_from_client_ps_cursor']);

    if (!$stmt = mysqli_stmt_init($link))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $id = null;
    if (!mysqli_stmt_prepare($stmt, 'SELECT id FROM test') ||
            !mysqli_stmt_execute($stmt) ||
            !mysqli_stmt_store_result($stmt) ||
            !mysqli_stmt_bind_result($stmt, $id))
        printf("[002] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $num_rows = 0;
    while (mysqli_stmt_fetch($stmt))
        $num_rows++;

    mysqli_stmt_free_result($stmt);

    $after = mysqli_get_client_stats();

    if ($after['rows_fetched_from_client_ps_unbuffered'] != $stats['rows_fetched_from_client_ps_unbuffered'])
        printf("[003] Unbuffered rows got increased after buffered PS, expecting %d got %d.\n",
            $stats['rows_fetched_from_client_ps_unbuffered'],
            $after['rows_fetched_from_client_ps_unbuffered']);

    $stats['rows_fetched_from_client_ps_buffered'] += $num_rows;
    if ($after['rows_fetched_from_client_ps_buffered'] != $stats['rows_fetched_from_client_ps_buffered'] )
        printf("[005] Buffered rows should be %d got %d.\n",
            $stats['rows_fetched_from_client_ps_buffered'],
            $after['rows_fetched_from_client_ps_buffered']);

    $stats = $after;
    printf("BUFFERED: rows_fetched_from_client_ps_unbuffered = %d\n",	$stats['rows_fetched_from_client_ps_unbuffered']);
    printf("BUFFERED: rows_fetched_from_client_ps_buffered = %d\n",	$stats['rows_fetched_from_client_ps_buffered']);
    printf("BUFFERED: rows_fetched_from_client_ps_cursor = %d\n",	$stats['rows_fetched_from_client_ps_cursor']);

    $id = null;
    if (!mysqli_stmt_prepare($stmt, 'SELECT id FROM test') ||
            !mysqli_stmt_execute($stmt) ||
            !mysqli_stmt_bind_result($stmt, $id))
        printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $num_rows = 0;
    while (mysqli_stmt_fetch($stmt))
        $num_rows++;

    $after = mysqli_get_client_stats();
    $stats['rows_fetched_from_client_ps_unbuffered'] += $num_rows;
    if ($after['rows_fetched_from_client_ps_unbuffered'] != $stats['rows_fetched_from_client_ps_unbuffered'])
        printf("[007] Unbuffered rows should be %d got %d.\n",
            $stats['rows_fetched_from_client_ps_unbuffered'],
            $after['rows_fetched_from_client_ps_unbuffered']);

    if ($after['rows_fetched_from_client_ps_buffered'] != $stats['rows_fetched_from_client_ps_buffered'] )
        printf("[005] Buffered rows should be unchanged, expecting %d got %d.\n",
            $stats['rows_fetched_from_client_ps_buffered'],
            $after['rows_fetched_from_client_ps_buffered']);

    mysqli_stmt_free_result($stmt);
    mysqli_stmt_close($stmt);

    $stats = $after;
    printf("UNBUFFERED: rows_fetched_from_client_ps_unbuffered = %d\n",	$stats['rows_fetched_from_client_ps_unbuffered']);
    printf("UNBUFFERED: rows_fetched_from_client_ps_buffered = %d\n",	$stats['rows_fetched_from_client_ps_buffered']);
    printf("UNBUFFERED: rows_fetched_from_client_ps_cursor = %d\n",	$stats['rows_fetched_from_client_ps_cursor']);

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
BEGINNING: rows_fetched_from_client_ps_unbuffered = %d
BEGINNING: rows_fetched_from_client_ps_buffered = %d
BEGINNING: rows_fetched_from_client_ps_cursor = 0
BUFFERED: rows_fetched_from_client_ps_unbuffered = %d
BUFFERED: rows_fetched_from_client_ps_buffered = %d
BUFFERED: rows_fetched_from_client_ps_cursor = 0
UNBUFFERED: rows_fetched_from_client_ps_unbuffered = %d
UNBUFFERED: rows_fetched_from_client_ps_buffered = %d
UNBUFFERED: rows_fetched_from_client_ps_cursor = 0
done!
