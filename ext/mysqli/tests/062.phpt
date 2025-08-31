--TEST--
mysqli_result constructor
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

$mysqli->real_query("SELECT 'foo' FROM DUAL");

$myresult = new mysqli_result($mysqli);

$row = $myresult->fetch_row();
$myresult->close();
$mysqli->close();

var_dump($row);

try {
    new mysqli_result($mysqli);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$mysqli = new mysqli();
try {
    new mysqli_result($mysqli);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}
print "done!";
?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "foo"
}
my_mysqli object is already closed
mysqli object is not fully initialized
done!
