--TEST--
mysqli_disable_reads_from_master()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_disable_reads_from_master')) {
	die("skip mysqli_disable_reads_from_master() not available");
}
?>
--FILE--
<?php
    require_once("connect.inc");

    $tmp    = NULL;
    $link   = NULL;

    if (NULL !== ($tmp = @mysqli_disable_reads_from_master()))
        printf("[001] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (NULL !== ($tmp = @mysqli_disable_reads_from_master($link)))
        printf("[002] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }

    if (!is_bool($tmp = mysqli_disable_reads_from_master($link)))
        printf("[004] Expecting boolean/[true|false] value, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_close($link);

    if (NULL !== ($tmp = mysqli_disable_reads_from_master($link)))
        printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_disable_reads_from_master(): mysqli object is already closed in %s on line %d
done!
