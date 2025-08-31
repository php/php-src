--TEST--
Test session_set_save_handler() function : error functionality
--EXTENSIONS--
session
--INI--
session.save_handler=
--FILE--
<?php
ob_start();

session_start();
session_set_save_handler(new \SessionHandler(), true);

echo "ok";
ob_end_flush();
?>
--EXPECTF--
Warning: session_start(): Cannot find session save handler "" - session startup failed in %s on line %d
ok
