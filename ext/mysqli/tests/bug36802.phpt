--TEST--
bug #36802 : crashes with mysql_init                                            
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

	class my_mysqli extends mysqli {
		function __construct() 
		{
		}
	}

	include "connect.inc";


	$mysql = mysqli_init();

	/* following operations should not work */
	$x[0] = @$mysql->set_charset('utf8');
	$x[1] = @$mysql->query("SELECT 'foo' FROM DUAL");

	/* following operations should work */ 
	$x[2] = ($mysql->client_version > 0);
	$x[3] = $mysql->errno;
	$mysql->close();
	


	var_dump($x);
?>
--EXPECT--
array(4) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  bool(true)
  [3]=>
  int(0)
}
