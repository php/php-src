--TEST--
Bug #74779 (x() and y() truncating floats to integers)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
if (!setlocale(LC_NUMERIC, "de_DE", "de_DE.UTF-8", "de-DE")) die('skip locale not available');
?>
--FILE--
<?php
require_once("connect.inc");

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
        $host, $user, $db, $port, $socket);
}

if (!setlocale(LC_NUMERIC, "de_DE", "de_DE.UTF-8", "de-DE")) {
    echo "[002] Cannot set locale\n";
}

if (!$link->options(MYSQLI_OPT_INT_AND_FLOAT_NATIVE, true)) {
    printf("[003] [%d] %s\n", $link->errno, $link->error);
}

if (!$result = $link->query("SELECT ST_Y(Point(56.7, 53.34))")) {
    printf("[004] [%d] %s\n", $link->errno, $link->error);
}

if (!$array = $result->fetch_array(MYSQLI_ASSOC)) {
    printf("[005] [%d] %s\n", $link->errno, $link->error);
}

var_dump($array);

mysqli_close($link);
?>
--EXPECT--
array(1) {
  ["ST_Y(Point(56.7, 53.34))"]=>
  float(53.34)
}
