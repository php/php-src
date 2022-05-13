--TEST--
resultset constructor
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    $stmt = new mysqli_stmt($mysql, "SELECT 'foo' FROM DUAL");
    $stmt->execute();
    $stmt->bind_result($foo);
    $stmt->fetch();
    $stmt->close();
    var_dump($foo);

    mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
    try {
        // an exception should be thrown from prepare (i.e. constructor) not from execute
        $stmt = new mysqli_stmt($mysql, "SELECT invalid FROM DUAL");
    } catch(mysqli_sql_exception $e) {
        echo $e->getMessage()."\n";
    }

    $mysql->close();
?>
--EXPECT--
string(3) "foo"
Unknown column 'invalid' in 'field list'
