--TEST--
Test session_set_save_handler() : calling default handler when save_handler=user
--INI--
session.save_handler=user
session.name=PHPSESSID
display_errors=off
error_log=
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : calling default handler when save_handler=user ***\n";

$oldHandler = ini_get('session.save_handler');
$handler = new SessionHandler;
session_set_save_handler($handler);

session_start();
--EXPECT--
PHP Recoverable fatal error:  PHP Startup: Cannot set 'user' save handler by ini_set() or session_module_name() in Unknown on line 0
*** Testing session_set_save_handler() : calling default handler when save_handler=user ***
