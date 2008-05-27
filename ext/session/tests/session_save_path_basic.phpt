--TEST--
Test session_save_path() function : basic functionality
--INI--
session.gc_probability=0
session.save_path=
session.name=PHPSESSID
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : string session_save_path([string $path])
 * Description : Get and/or set the current session save path
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_save_path() : error functionality ***\n";

$directory = dirname(__FILE__);
var_dump(session_save_path());
var_dump(session_save_path($directory));
var_dump(session_save_path());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_save_path() : error functionality ***
unicode(0) ""
unicode(0) ""
unicode(%d) "%s"
Done

