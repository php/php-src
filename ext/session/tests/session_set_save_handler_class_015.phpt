--TEST--
Test session_set_save_handler() : register session handler but don't start
--INI--
session.save_handler=files
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

echo "*** Testing session_set_save_handler() : register session handler but don't start  ***\n";

session_set_save_handler(new SessionHandler);
--EXPECTF--
*** Testing session_set_save_handler() : register session handler but don't start  ***
