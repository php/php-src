--TEST--
mysqli connect
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$dbname = "test";
	$test = "";

	/*** test mysqli_connect localhost:port ***/
	$link = mysqli_connect($host, $user, $passwd, "", 3306);
	$test .= ($link) ? "1" : "0";
	mysqli_close($link);

	/*** test mysqli_real_connect ***/
	$link = mysqli_init();	
	$test.= (mysqli_real_connect($link, $host, $user, $passwd)) 
		? "1" : "0";
	mysqli_close($link);

	/*** test mysqli_real_connect with db ***/
	$link = mysqli_init();	
	$test .= (mysqli_real_connect($link, $host, $user, $passwd, $dbname)) 
		? "1" : "0";
	mysqli_close($link);

	/*** test mysqli_real_connect with port ***/
	$link = mysqli_init();	
	$test .= (mysqli_real_connect($link, $host, $user, $passwd, $dbname, 3306))
		? "1":"0";
	mysqli_close($link);

	/*** test mysqli_real_connect compressed ***/
	$link = mysqli_init();	
	$test .= (mysqli_real_connect($link, $host, $user, $passwd, $dbname, 0, NULL, MYSQLI_CLIENT_COMPRESS)) 
		? "1" : "0";
	mysqli_close($link);

	/* todo ssl connections */

	var_dump($test);
?>
--EXPECT--
string(5) "11111"
