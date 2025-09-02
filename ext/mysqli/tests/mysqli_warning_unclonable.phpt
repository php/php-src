--TEST--
Trying to clone mysqli_warning object
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!mysqli_query($link, "SET sql_mode=''"))
        printf("[002] Cannot set SQL-Mode, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    
    if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "CREATE TABLE test (id SMALLINT NOT NULL)"))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test (id) VALUES (1), (NULL)"))
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($warning = mysqli_get_warnings($link)) || 'mysqli_warning' != get_class($warning)) {
        printf("[006] Expecting object/mysqli_warning, got %s/%s\n", gettype($warning), (is_object($warning) ? var_export($warning, true) : $warning));
    }

    $warning_clone = clone $warning;
    print "done!";
?>
--CLEAN--
<?php
require_once 'clean_table.inc';
?>
--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class mysqli_warning in %s:%d
Stack trace:%A
