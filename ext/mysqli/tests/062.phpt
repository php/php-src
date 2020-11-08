--TEST--
resultset constructor
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    $mysql->real_query("SELECT 'foo' FROM DUAL");

    $myresult = new mysqli_result($mysql);

    $row = $myresult->fetch_row();
    $myresult->close();
    $mysql->close();

    var_dump($row);
    print "done!";
?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "foo"
}
done!
