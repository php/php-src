--TEST--
Test session_set_save_handler() function : variation
--INI--
session.use_strict_mode=1
session.gc_probability=1
session.gc_divisor=1
session.gc_maxlifetime=0
session.save_path=
session.name=PHPSESSID
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_set_save_handler(callback $open, callback $close, callback $read, callback $write, callback $destroy, callback $gc)
 * Description : Sets user-level session storage functions with validate_id() and update()
 * Source code : ext/session/session.c
 */

function noisy_gc($maxlifetime) {
	echo("GC [".$maxlifetime."]\n");
	echo gc($maxlifetime)." deleted\n";
	return true;
}

echo "*** Testing session_set_save_handler() : variation ***\n";

/***
Note: Besides maxlifetime setting is ignored, custome save handler deletes
session data file always when it is called becuase of gc_probability and
gc_divisor are 1. gc is called always. Because of this, session data file
does not exists always and session ID validation fails always. As a result
create ID function is called always.
***/

require_once "save_handler.inc";
$path = dirname(__FILE__);
var_dump(session_save_path($path));

echo "*** Without lazy_write ***\n";
var_dump(session_set_save_handler("open", "close", "read", "write", "destroy", "noisy_gc", "create_sid", "validate_sid", "update"));
var_dump(session_start(['lazy_write'=>FALSE]));
$session_id = session_id();
var_dump(session_id());
var_dump(session_write_close());
var_dump(session_id());

echo "*** With lazy_write ***\n";
var_dump(session_id($session_id));
var_dump(session_set_save_handler("open", "close", "read", "write", "destroy", "noisy_gc", "create_sid", "validate_sid", "update"));
var_dump(session_start(['lazy_write'=>TRUE]));
var_dump(session_commit());
var_dump(session_id());

echo "*** Cleanup ***\n";
var_dump(session_id($session_id));
var_dump(session_start());
var_dump(session_destroy(true));

ob_end_flush();

?>
--EXPECTF--
*** Testing session_set_save_handler() : variation ***

string(0) ""
*** Without lazy_write ***
bool(true)
Open [%s,PHPSESSID]
GC [0]
1 deleted
CreateID [PHPT-%d]
Read [%s,PHPT-%d]
bool(true)
string(%d) "PHPT-%d"
Write [%s,PHPT-%d,__PHP_SESSION__|a:3:{s:7:"CREATED";i:%d;s:7:"UPDATED";i:%d;s:4:"SIDS";a:0:{}}]
Close [%s,PHPSESSID]
NULL
string(%d) "PHPT-%d"
*** With lazy_write ***
string(%d) "PHPT-%d"
bool(true)
Open [%s,PHPSESSID]
GC [0]
1 deleted
ValidateID [%s,PHPT-%d]
CreateID [PHPT-%d]
Read [%s,PHPT-%d]
bool(true)
Write [%s,PHPT-%d,__PHP_SESSION__|a:3:{s:7:"CREATED";i:%d;s:7:"UPDATED";i:%d;s:4:"SIDS";a:0:{}}]
Close [%s,PHPSESSID]
NULL
string(%d) "PHPT-%d"
*** Cleanup ***
string(%d) "PHPT-%d"
Open [%s,PHPSESSID]
GC [0]
1 deleted
ValidateID [%s,PHPT-%d]
CreateID [PHPT-%d]
Read [%s,PHPT-%d]
bool(true)
Destroy [%s,PHPT-%d]
Close [%s,PHPSESSID]
bool(true)
