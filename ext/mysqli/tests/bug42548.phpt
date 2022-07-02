--TEST--
Bug #42548 PROCEDURE xxx can't return a result set in the given context (works in 5.2.3!!)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';
if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf('skip Cannot connect to MySQL, [%d] %s.', mysqli_connect_errno(), mysqli_connect_error()));
}
if (mysqli_get_server_version($link) <= 50000) {
    die(sprintf('skip Needs MySQL 5.0+, found version %d.', mysqli_get_server_version($link)));
}
?>
--FILE--
<?php
require_once 'connect.inc';

$mysqli = mysqli_init();
$mysqli->real_connect($host, $user, $passwd, $db, $port, $socket);
if (mysqli_connect_errno()) {
    printf("Connect failed: %s\n", mysqli_connect_error());
    exit();
}

$mysqli->query("DROP PROCEDURE IF EXISTS p1") or die($mysqli->error);
$mysqli->query("CREATE PROCEDURE p1() BEGIN SELECT 23; SELECT 42; END") or die($mysqli->error);

if ($mysqli->multi_query("CALL p1();"))
{
    do
    {
        if ($objResult = $mysqli->store_result()) {
            while ($row = $objResult->fetch_assoc()) {
                print_r($row);
            }
            $objResult->close();
            if ($mysqli->more_results()) {
                print "----- next result -----------\n";
            }
        } else {
            print "no results found\n";
        }
    } while ($mysqli->more_results() && $mysqli->next_result());
} else {
    print $mysqli->error;
}

$mysqli->query("DROP PROCEDURE p1") or die($mysqli->error);
$mysqli->close();
print "done!";
?>
--CLEAN--
<?php
require_once "connect.inc";
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

mysqli_query($link, "DROP PROCEDURE IF EXISTS p1");

mysqli_close($link);
?>
--EXPECT--
Array
(
    [23] => 23
)
----- next result -----------
Array
(
    [42] => 42
)
----- next result -----------
no results found
done!
