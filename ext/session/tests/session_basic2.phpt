--TEST--
Test basic function : variation2
--INI--
session.use_strict_mode=1
session.save_handler=files
session.hash_bits_per_character=4
session.gc_probability=1
session.gc_divisor=1000
session.gc_maxlifetime=300
session.save_path=
session.sid_length=32
session.name=PHPSESSID
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : session.use_strict_mode=1
 * Description : Test basic functionality.
 * Source code : ext/session/session.c, ext/session/mod_files.c
 */

echo "*** Testing basic session functionality : variation2 ***\n";

$session_id = 'testid';
session_id($session_id);
$path = __DIR__;
var_dump(session_save_path($path));

echo "*** Without lazy_write ***\n";
var_dump(session_id($session_id));
var_dump(session_start(['lazy_write'=>FALSE]));
$session_id_new1 = session_id();
var_dump($session_id_new1 !== $session_id);
var_dump(session_write_close());
var_dump(session_id());

echo "*** With lazy_write ***\n";
var_dump(session_id($session_id));
var_dump(session_start(['lazy_write'=>TRUE]));
$session_id_new2 = session_id();
var_dump($session_id_new1 !== $session_id_new2);
var_dump(session_commit());
var_dump(session_id());

echo "*** Cleanup ***\n";
ini_set('session.use_strict_mode',0);
var_dump(session_id($session_id_new1));
var_dump(session_start());
var_dump(session_destroy());
var_dump(session_id($session_id_new2));
var_dump(session_start());
var_dump(session_destroy());

ob_end_flush();
?>
--EXPECTF--
*** Testing basic session functionality : variation2 ***
string(0) ""
*** Without lazy_write ***
string(6) "testid"
bool(true)
bool(true)
bool(true)
string(32) "%s"
*** With lazy_write ***
string(32) "%s"
bool(true)
bool(true)
bool(true)
string(32) "%s"
*** Cleanup ***
string(32) "%s"
bool(true)
bool(true)
string(0) ""
bool(true)
bool(true)
