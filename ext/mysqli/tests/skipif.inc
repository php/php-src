<?php
if (!extension_loaded('mysqli')){
	die('skip mysqli extension not available');
}
include "connect.inc";
$driver = new mysqli_driver();
if (!$driver->embedded && !($con = @mysqli_connect($host, $user, $passwd, "", 3306))) {
	die('skip could not connect to MySQL');
}
?>
