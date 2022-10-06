--TEST--
Bug #49422 (mysqlnd: mysqli_real_connect() and LOAD DATA INFILE crash)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';

$link = mysqli_init();
if (!@my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
}

include_once "local_infile_tools.inc";
if ($msg = check_local_infile_support($link, $engine))
    die(sprintf("skip %s, [%d] %s", $msg, $link->errno, $link->error));

mysqli_close($link);
?>
--INI--
mysqli.allow_local_infile=1
mysqli.allow_persistent=1
mysqli.max_persistent=1
--FILE--
<?php
    include "connect.inc";

    $link = mysqli_init();
    if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }

    if (!mysqli_query($link, 'DROP TABLE IF EXISTS test')) {
        printf("[002] Failed to drop old test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    if (!mysqli_query($link, 'CREATE TABLE test(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE=' . $engine)) {
        printf("[003] Failed to create test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    include("local_infile_tools.inc");
    $file = create_standard_csv(4);

    if (!@mysqli_query($link, sprintf("LOAD DATA LOCAL INFILE '%s'
            INTO TABLE test
            FIELDS TERMINATED BY ';' OPTIONALLY ENCLOSED BY '\''
            LINES TERMINATED BY '\n'",
            mysqli_real_escape_string($link, $file)))) {
            printf("[005] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));
    }

    if (!$res = mysqli_query($link, "SELECT * FROM test ORDER BY id"))
        printf("[006] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

    $rows = array();
    while ($row = mysqli_fetch_assoc($res)) {
        var_dump($row);
        $rows[] = $row;
    }

    mysqli_free_result($res);

    mysqli_query($link, "DELETE FROM test");
    mysqli_close($link);

    /*
        mysqlnd makes a connection created through mysql_init()/mysqli_real_connect() always a 'persistent' one.
        At this point 'persistent' is not to be confused with what a user calls a 'persistent' - in this case
        'persistent' means that mysqlnd uses malloc() instead of emalloc(). nothing else. ext/mysqli will
        not consider it as a 'persistent' connection in a user sense, ext/mysqli will not apply max_persistent etc.
        It's only about malloc() vs. emalloc().

        However, the bug is about malloc() and efree(). You can make mysqlnd use malloc() by either using
        pconnect or mysql_init() - so we should test pconnect as well.
    */
    $host = 'p:' . $host;
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[007] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }

    /* bug happened during query processing */
    if (!@mysqli_query($link, sprintf("LOAD DATA LOCAL INFILE '%s'
        INTO TABLE test
        FIELDS TERMINATED BY ';' OPTIONALLY ENCLOSED BY '\''
        LINES TERMINATED BY '\n'",
        mysqli_real_escape_string($link, $file)))) {
        printf("[008] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));
    }

    /* we survived? that's good enough... */

    if (!$res = mysqli_query($link, "SELECT * FROM test ORDER BY id"))
        printf("[009] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

    $i = 0;
    while ($row = mysqli_fetch_assoc($res)) {
        if (($row['id'] != $rows[$i]['id']) || ($row['label'] != $rows[$i]['label'])) {
            printf("[010] Wrong values, check manually!\n");
        }
        $i++;
    }
    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
require_once "clean_table.inc";
?>
--EXPECT--
array(2) {
  ["id"]=>
  string(2) "97"
  ["label"]=>
  string(1) "x"
}
array(2) {
  ["id"]=>
  string(2) "98"
  ["label"]=>
  string(1) "y"
}
array(2) {
  ["id"]=>
  string(2) "99"
  ["label"]=>
  string(1) "z"
}
done!
