--TEST--
extend mysqli 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	class foobar extends mysqli {
		function test () {
			return ("I like MySQL 4.1");
		}
	}

	$foo = new foobar();
	$foo->connect($host, $user, $passwd);
	$foo->close();
	printf("%s\n", $foo->test());
?>
--EXPECT--
I like MySQL 4.1
