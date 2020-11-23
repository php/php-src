--TEST--
Bug #70949 (SQL Result Sets With NULL Can Cause Fatal Memory Errors)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once("connect.inc");
if (!$IS_MYSQLND) {
    die("skip mysqlnd only test");
}
?>
--FILE--
<?php
require_once("connect.inc");
$mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

$mysql->query("DROP TABLE IF EXISTS bug70949");
$mysql->query("CREATE TABLE bug70949(name varchar(255))");
$mysql->query("INSERT INTO bug70949 VALUES ('dummy'),(NULL),('foo'),('bar')");

$sql = "select * from bug70949";

if ($stmt = $mysql->prepare($sql))
{
    $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_READ_ONLY);

    if ($stmt->bind_result($name)) {
        {
            if ($stmt->execute())
            {
                while ($stmt->fetch())
                {
                    var_dump($name);
                }
            }
        }

        $stmt->free_result();
        $stmt->close();
    }


    $mysql->close();
}

?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS bug70949"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
string(5) "dummy"
NULL
string(3) "foo"
string(3) "bar"
