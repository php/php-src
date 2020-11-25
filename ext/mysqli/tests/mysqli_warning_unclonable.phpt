--TEST--
Trying to clone mysqli_warning object
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');
if (!$TEST_EXPERIMENTAL)
    die("skip - experimental (= unsupported) feature");
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "CREATE TABLE test (id SMALLINT)"))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test (id) VALUES (1000000)"))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($warning = mysqli_get_warnings($link)) || 'mysqli_warning' != get_class($warning)) {
        printf("[005] Expecting object/mysqli_warning, got %s/%s\n", gettype($tmp), (is_object($tmp) ? var_dump($tmp, true) : $tmp));
    }

    $warning_clone = clone $warning;
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
Fatal error: Trying to clone an uncloneable object of class mysqli_warning in %s on line %d
