--TEST--
GH-13856 (Member access within null pointer of type 'ps_files' in ext/session/mod_files.c)
--EXTENSIONS--
session
--INI--
session.save_handler=files
open_basedir=.
error_reporting=E_ALL
--FILE--
<?php
session_set_save_handler(new \SessionHandler(), true);
session_start();
?>
--EXPECTF--
Warning: SessionHandler::open(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (.) in %s on line %d

Warning: SessionHandler::close(): Parent session handler is not open in %s on line %d

Warning: session_start(): Failed to initialize storage module: user (path: ) in %s on line %d
