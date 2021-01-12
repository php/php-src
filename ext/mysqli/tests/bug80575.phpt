--TEST--
Bug #80575 (Casting mysqli to array produces null on every field)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once('connect.inc');
$link = new mysqli($host, $user, $passwd, $db, $port, $socket);
var_dump(((array) $link)["affected_rows"]);
var_dump($link->__debugInfo()["affected_rows"]);
?>
--EXPECT--
int(0)
int(0)
