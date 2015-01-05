--TEST--
Test session_set_save_handler() : calling default handler when save_handler=user
--INI--
session.save_handler=user
session.name=PHPSESSID
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_set_save_handler(SessionHandler $handler [, bool $register_shutdown_function = true])
 * Description : Sets user-level session storage functions
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_set_save_handler() : calling default handler when save_handler=user ***\n";

$oldHandler = ini_get('session.save_handler');
$handler = new SessionHandler;
session_set_save_handler($handler);

session_start();

--EXPECTF--
*** Testing session_set_save_handler() : calling default handler when save_handler=user ***

Fatal error: SessionHandler::open(): Cannot call default session handler in %s on line %d
