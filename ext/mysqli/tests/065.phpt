--TEST--
set character set 
--SKIPIF--
<?php 
require_once('skipif.inc'); 
if (!function_exists('mysqli_set_charset')) {
	die('skip mysqli_set_charset() not available');
}
if (!mysqli_set_charset($con, "gbh") && mysqli_errno($con) == 2019) {
    die('skip mysql compiled without gbh charset support');
}
?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd);
	mysqli_query($mysql, "SET sql_mode=''");

	$esc_str = chr(0xbf) . chr(0x5c);

	if ($mysql->set_charset("latin1")) {
		/* 5C should be escaped */
		$len[0] = strlen($mysql->real_escape_string($esc_str));
		$charset[0] = $mysql->client_encoding();
	}

	if ($mysql->set_charset("gbk")) {
		/* nothing should be escaped, it's a valid gbk character */
		$len[1] = strlen($mysql->real_escape_string($esc_str));
		$charset[1] = $mysql->client_encoding();
	}

	$mysql->close();
	var_dump($len[0]);
	var_dump($len[1]);
	var_dump($charset[0]);
	var_dump($charset[1]);
?>
--EXPECT--
int(3)
int(2)
string(6) "latin1"
string(3) "gbk"
