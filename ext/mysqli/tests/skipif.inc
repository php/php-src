<?php
if (!extension_loaded('mysqli')){
	die('skip mysqli extension not available');
}
include "connect.inc";
if (!@mysqli_connect($host, $user, $passwd, "", 3306)) {
	die('skip could not connect to MySQL');
}
?>
