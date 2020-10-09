--TEST--
mysqli_more_results()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    require('table.inc');

    print "[004]\n";
    var_dump(mysqli_more_results($link));

    if (!mysqli_multi_query($link, "SELECT 1 AS a; SELECT 1 AS a, 2 AS b; SELECT id FROM test ORDER BY id LIMIT 3"))
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    print "[006]\n";
    $i = 1;

    if (mysqli_get_server_version($link) > 41000 && !($ret = mysqli_more_results($link)))
        printf("[007] Expecting boolean/true, got %s/%s\n", gettype($ret), $ret);
    do {
        $res = mysqli_store_result($link);
        mysqli_free_result($res);
        if (mysqli_more_results($link))
            printf("%d\n", $i++);
    } while (mysqli_next_result($link));

    if (!mysqli_multi_query($link, "SELECT 1 AS a; SELECT 1 AS a, 2 AS b; SELECT id FROM test ORDER BY id LIMIT 3"))
        printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    print "[010]\n";
    $i = 1;
    if (mysqli_get_server_version($link) > 41000 && !($ret = mysqli_more_results($link)))
        printf("[011] Expecting boolean/true, got %s/%s\n", gettype($ret), $ret);

    do {
        $res = mysqli_use_result($link);
        // NOTE: if you use mysqli_use_result() with mysqli_more_results() or any other info function,
        // you must fetch all rows before you can loop to the next result set!
        // See also the MySQL Reference Manual: mysql_use_result()
        while ($row = mysqli_fetch_array($res))
            ;
        mysqli_free_result($res);
        if (mysqli_more_results($link))
            printf("%d\n", $i++);
    } while (mysqli_next_result($link));

    mysqli_close($link);

    try {
        mysqli_more_results($link);
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
[004]
bool(false)
[006]
1
2
[010]
1
2
mysqli object is already closed
done!
