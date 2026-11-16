--TEST--
Session IDs with NUL bytes are rejected as a whole, not truncated
--INI--
session.save_handler=files
session.save_path=
session.use_cookies=0
session.use_strict_mode=0
--EXTENSIONS--
session
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
session_id("nulbug\0stillvalid");
$started = session_start();
var_dump($started);
var_dump(session_id());
if ($started) {
    session_destroy();
}
?>
--EXPECTF--
Warning: session_start(): Session ID is too long or contains illegal characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d

Warning: session_start(): Failed to read session data: files (path: ) in %s on line %d
bool(false)
string(0) ""
