--TEST--
session GC correctly cleans expired sessions when save_path dirdepth > 0
--EXTENSIONS--
session
--SKIPIF--
<?php include(__DIR__ . '/../skipif.inc'); ?>
--INI--
session.gc_probability=0
session.gc_maxlifetime=1
--FILE--
<?php

$base = __DIR__ . '/gc_dirdepth_test';
@mkdir($base);
@mkdir("$base/a");

// ── Part 1: dirdepth=1
session_save_path("1;$base");

$stale_id   = 'abcdefghijklmnopqrstuvwx';
$stale_file = "$base/a/sess_$stale_id";
file_put_contents($stale_file, 'user|s:5:"alice";');
touch($stale_file, time() - 100);   // 100 s old; gc_maxlifetime=1 → must be GC'd

session_id('a0000000000000000000000000');
session_start();
$result_depth = session_gc();
session_destroy();
$depth_file_gone = !file_exists($stale_file);

// ── Part 2: dirdepth=0
session_save_path($base);

$flat_id   = 'bbcdefghijklmnopqrstuvwx';
$flat_file = "$base/sess_$flat_id";
file_put_contents($flat_file, 'user|s:5:"alice";');
touch($flat_file, time() - 100);

session_start();
$result_flat = session_gc();
session_destroy();
$flat_file_gone = !file_exists($flat_file);

echo "dirdepth=1 — session_gc() return value: ";
var_dump($result_depth);

echo "dirdepth=1 — expired session file removed: ";
var_dump($depth_file_gone);

echo "dirdepth=0 — session_gc() return value: ";
var_dump($result_flat);

echo "dirdepth=0 — expired session file removed: ";
var_dump($flat_file_gone);
?>
--CLEAN--
<?php
$base = __DIR__ . '/gc_dirdepth_test';
@unlink("$base/a/sess_abcdefghijklmnopqrstuvwx");
@unlink("$base/a/sess_a0000000000000000000000000");
@rmdir("$base/a");
@rmdir($base);
?>
--EXPECT--
dirdepth=1 — session_gc() return value: int(1)
dirdepth=1 — expired session file removed: bool(true)
dirdepth=0 — session_gc() return value: int(1)
dirdepth=0 — expired session file removed: bool(true)
