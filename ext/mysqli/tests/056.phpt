--TEST--
extend mysqli 
--FILE--
<?php
	include "connect.inc";

	class foobar extends mysqli {
		function test () {
			return ("I like MySQL 4.1");
		}
	}

	$foo = new foobar();
	$foo->connect("localhost", $user, $passwd);
	$foo->close();
	printf("%s\n", $foo->test());
?>
--EXPECT--
I like MySQL 4.1
