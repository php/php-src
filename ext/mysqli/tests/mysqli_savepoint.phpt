--TEST--
mysqli_savepoint()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';
if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

if (!have_innodb($link))
    die(sprintf("skip Needs InnoDB support, [%d] %s", $link->errno, $link->error));
?>
--FILE--
<?php
    require_once "connect.inc";

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    try {
        mysqli_savepoint($link, '');
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    if (!mysqli_query($link, 'DROP TABLE IF EXISTS test'))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, 'CREATE TABLE test(id INT) ENGINE = InnoDB'))
        printf("[008] Cannot create test table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (true !== ($tmp = mysqli_autocommit($link, false)))
        printf("[009] Cannot turn off autocommit, expecting true, got %s/%s\n", gettype($tmp), $tmp);

    /* overrule autocommit */
    if (true !== ($tmp = mysqli_savepoint($link, 'my')))
        printf("[010] Got %s - [%d] %s\n", var_dump($tmp, true), mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, 'INSERT INTO test(id) VALUES (1)'))
        printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $tmp = mysqli_rollback($link);
    if ($tmp !== true)
        printf("[012] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    print "done!";
?>
--CLEAN--
<?php
require_once "clean_table.inc";
?>
--EXPECT--
mysqli_savepoint(): Argument #2 ($name) cannot be empty
done!
