--TEST--
Test basic function : variation1
--INI--
session.use_strict_mode=0
session.save_handler=files
session.gc_probability=1
session.gc_divisor=1000
session.gc_maxlifetime=300
session.save_path=
session.name=PHPSESSID
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing basic session functionality : variation1 ***\n";

$session_id = 'session-basic1';
session_id($session_id);
$path = __DIR__;
var_dump(session_save_path($path));

echo "*** Without lazy_write ***\n";
var_dump(session_id($session_id));
$config = ['lazy_write'=>FALSE];
var_dump(session_start($config));
var_dump($config);
var_dump(session_write_close());
var_dump(session_id());

echo "*** With lazy_write ***\n";
var_dump(session_id($session_id));
var_dump(session_start(['lazy_write'=>TRUE]));
var_dump(session_commit());
var_dump(session_id());

echo "*** Cleanup ***\n";
var_dump(session_id($session_id));
var_dump(session_start());
var_dump(session_destroy());

ob_end_flush();
?>
--EXPECT--
*** Testing basic session functionality : variation1 ***
string(0) ""
*** Without lazy_write ***
string(14) "session-basic1"
bool(true)
array(1) {
  ["lazy_write"]=>
  bool(false)
}
bool(true)
string(14) "session-basic1"
*** With lazy_write ***
string(14) "session-basic1"
bool(true)
bool(true)
string(14) "session-basic1"
*** Cleanup ***
string(14) "session-basic1"
bool(true)
bool(true)
