--TEST--
mysqli_prepare() called on a closed connection should return FALSE (bug #75448)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once 'connect.inc';
$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);
mysqli_close($link);
$stmt = mysqli_prepare($link, 'SELECT VERSION()');
var_dump($stmt);
?>
--EXPECTF--
Warning: mysqli_prepare(): Couldn't fetch mysqli in %s on line %d
bool(false)
