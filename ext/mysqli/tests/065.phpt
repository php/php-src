--TEST--
set character set 
--SKIPIF--
<?php 
require_once('skipif.inc'); 
?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd);

	if ($mysql->set_client_encoding("utf8")) {
		var_dump($mysql->client_encoding());
	}
	$mysql->close();
?>
--EXPECT--
string(4) "utf8"
