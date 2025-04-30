--TEST--
mysqli_get_client_stats() - implicit_free_result
--EXTENSIONS--
mysqli
--SKIPIF--
<?PHP
require_once 'skipifconnectfailure.inc';
?>
--INI--
mysqlnd.collect_statistics=1
mysqlnd.collect_memory_statistics=1
--FILE--
<?php
    require_once 'connect.inc';
    require_once 'table.inc';

    $stats = mysqli_get_client_stats();
    printf("BEGINNING: implicit_free_result = %d\n",	$stats['implicit_free_result']);

    if (!$res = mysqli_query($link, 'SELECT id FROM test'))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    mysqli_free_result($res);
    mysqli_close($link);

    $after = mysqli_get_client_stats();
    if ($after['implicit_free_result'] != $stats['implicit_free_result'])
        printf("[002] Where is the missing mysqli_free_result() call? implicit_free_result has changed by %d.\n",
            $after['implicit_free_result'] - $stats['implicit_free_result']);

    $stats = $after;
    printf("END: implicit_free_result = %d\n",	$stats['implicit_free_result']);

    print "done!";
?>
--CLEAN--
<?php
	require_once 'clean_table.inc';
?>
--EXPECTF--
BEGINNING: implicit_free_result = %d
END: implicit_free_result = %d
done!
