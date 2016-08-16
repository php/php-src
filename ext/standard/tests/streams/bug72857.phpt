--TEST--
Bug #72857 stream_socket_recvfrom read access violation
--FILE--
<?php
	$fp0 = fopen('stream_socket_recvfrom.tmp', 'w');
	$v2=10;
	$v3=STREAM_PEEK;
	$v4="A";

	var_dump(stream_socket_recvfrom($fp0,$v2,$v3,$v4), $v4); 
?>
==DONE==
--EXPECT--
string(0) ""
NULL
==DONE==
