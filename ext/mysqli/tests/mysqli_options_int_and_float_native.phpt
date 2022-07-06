--TEST--
mysqli_options() - MYSQLI_OPT_INT_AND_FLOAT_NATIVE
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');

require_once('connect.inc');
if (!$IS_MYSQLND)
    die("skip mysqlnd only test");
?>
--FILE--
<?php
    require_once("connect.inc");


    $types = array(
        'BIT' 			=> array('BIT(8)', 0),
        'TINYINT'		=> array('TINYINT', 120),
        'BOOL'			=> array('BOOL', 0),
        'BOOLEAN'		=> array('BOOLEAN', 1),
        'SMALLINT'		=> array('SMALLINT', 32000),
        'MEDIUMINT'		=> array('MEDIUMINT', 999),
        'INT'			=> array('INT', 999),
        'BIGINT'		=> array('BIGINT', 999),
        'FLOAT'			=> array('FLOAT', 1.3),
        'DOUBLE'		=> array('DOUBLE', -1.3),
    );

    foreach ($types as $name => $data) {
        $link = mysqli_init();
        if (!mysqli_options($link, MYSQLI_OPT_INT_AND_FLOAT_NATIVE, 1)) {
            printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
            continue;
        }

        if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
            printf("[002] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
            continue;
        }


        if (!mysqli_query($link, "DROP TABLE IF EXISTS test")) {
            printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
            continue;
        }

        if (!mysqli_query($link, sprintf("CREATE TABLE test (id %s)", $data[0]))) {
            printf("[004] TODO [%d] %s\n", mysqli_errno($link), mysqli_error($link));
            continue;
        }

        if (!mysqli_query($link, sprintf("INSERT INTO test(id) VALUES (%f)", $data[1]))) {
            printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
            continue;
        }

        if (!$res = mysqli_query($link, "SELECT id FROM test")) {
            printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
            continue;
        }

        $row = mysqli_fetch_assoc($res);
        mysqli_free_result($res);

        if ($row['id'] !== $data[1]) {
            printf("[007] Expecting %s - %s/%s got %s/%s\n",
                $name,
                $data[1], gettype($data[1]), $row['id'], gettype($row['id']));
        }
        mysqli_close($link);

        $link = mysqli_init();
        if (!mysqli_options($link, MYSQLI_OPT_INT_AND_FLOAT_NATIVE, 0)) {
            printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
            continue;
        }

        if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
            printf("[009] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
            continue;
        }

        if (!$res = mysqli_query($link, "SELECT id FROM test")) {
            printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
            continue;
        }

        $row = mysqli_fetch_assoc($res);
        mysqli_free_result($res);

        if (!is_string($row['id']) || ($row['id'] != $data[1])) {
            printf("[011] Expecting %s - %s/string got %s/%s\n",
                $name,
                $data[1], $row['id'], gettype($row['id']));
        }
        mysqli_close($link);

    }

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!
