--TEST--
setting $_SESSION before session_start() should not cause segfault
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php

error_reporting(E_ALL);

class Kill {
	function Kill() {
		global $HTTP_SESSION_VARS;
		session_start();
	}
}
$k = new Kill();

print "I live\n";
?>
--EXPECT--
I live
