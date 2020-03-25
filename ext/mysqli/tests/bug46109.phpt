--TEST--
Bug #46109 (MySQLi::init - Memory leaks)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $mysqli = new mysqli();
    $mysqli->init();
    $mysqli->init();
    echo "done";
?>
--EXPECT--
done
