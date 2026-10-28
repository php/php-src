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
// This single script calls session_start() several times. With cookies and
// the cache limiter enabled, session_start() refuses to run once any
// output has already been sent, so disable both and defer all var_dump()
// output to the very end of the script.
ini_set('session.use_cookies', 0);
ini_set('session.cache_limiter', '');
// gc_maxlifetime cannot be changed once a session is active, so set it
// before the first session_start() call; it is irrelevant to the "victim"
// session below, only to the "attacker" one further down.
ini_set('session.gc_maxlifetime', 1);
// session.use_strict_mode (default on) rejects a caller-supplied ID that
// does not already have a session file and silently replaces it with a
// freshly generated one. This test depends on session_id() actually
// pinning $victimId/'attackerxxx...', so strict mode must be off.
ini_set('session.use_strict_mode', 0);

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
session_gc();
session_write_close();

$survived_while_locked = file_exists($victimFile);

flock($lockHandle, LOCK_UN);
fclose($lockHandle);

// The data must still be there once nobody holds the lock any more.
session_id($victimId);
session_start();
$restored_session = $_SESSION;
session_write_close();

var_dump($survived_while_locked);
var_dump($restored_session);
?>
--EXPECT--
bool(true)
array(1) {
  ["secret"]=>
  string(12) "still-in-use"
}
