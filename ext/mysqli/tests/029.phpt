--TEST--
function test: mysqli_affected_rows
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    mysqli_select_db($link, $db);

    mysqli_query($link, "DROP TABLE IF EXISTS general_test");
    mysqli_query($link, "CREATE TABLE general_test (a INT)");
    mysqli_query($link, "INSERT INTO general_test VALUES (1),(2),(3)");

    $afc = mysqli_affected_rows($link);

    var_dump($afc);

    mysqli_query($link, "DROP TABLE IF EXISTS general_test");
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS general_test"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
int(3)
done!
