--TEST--
session_start() with partitioned cookies
--INI--
session.use_strict_mode=0
session.save_handler=files
session.save_path=
session.cookie_secure=0
session.cookie_partitioned=0
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

var_dump(session_start(['cookie_partitioned' => true]));
var_dump(session_start(['cookie_partitioned' => true, 'cookie_secure' => false]));
var_dump(session_start(['cookie_partitioned' => true, 'cookie_secure' => true]));

ob_end_flush();

?>
--EXPECTF--
Warning: session_start(): Partitioned session cookie cannot be used without also configuring it as secure in %s on line %d
bool(false)

Warning: session_start(): Partitioned session cookie cannot be used without also configuring it as secure in %s on line %d
bool(false)
bool(true)
