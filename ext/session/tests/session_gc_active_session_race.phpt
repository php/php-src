--TEST--
session_gc() must not delete a session file another request still has locked
--EXTENSIONS--
session
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) === 'WIN') die('skip flock()/unlink() interaction differs on Windows');
?>
--FILE--
<?php
$save_path = sys_get_temp_dir() . '/' . uniqid('ps_gc_lock_test_', true);
mkdir($save_path);
ini_set('session.save_path', $save_path);
ini_set('session.save_handler', 'files');

// The "victim": a session file a still-active request has open+locked, the
// way ps_files_open() holds flock(LOCK_EX) for as long as a slow request
// (large upload, slow payment callback, ...) is running.
$victimId = 'victimxxxxxxxxxxxxxxxxxxxxxxxxx';
session_id($victimId);
session_start();
$_SESSION['secret'] = 'still-in-use';
session_write_close();

$victimFile = $save_path . '/sess_' . $victimId;
// Make it look idle long enough to be GC-eligible...
touch($victimFile, time() - 100000);
// ...while keeping it open and locked, simulating the owning request still
// being alive.
$lockHandle = fopen($victimFile, 'r+');
flock($lockHandle, LOCK_EX);

// Forcing GC only requires an unrelated session of one's own.
session_id('attackerxxxxxxxxxxxxxxxxxxxxxxxx');
session_start();
ini_set('session.gc_maxlifetime', 1);
session_gc();
session_write_close();

var_dump(file_exists($victimFile));

flock($lockHandle, LOCK_UN);
fclose($lockHandle);

// The data must still be there once nobody holds the lock any more.
session_id($victimId);
session_start();
var_dump($_SESSION);
session_write_close();
?>
--EXPECT--
bool(true)
array(1) {
  ["secret"]=>
  string(12) "still-in-use"
}
