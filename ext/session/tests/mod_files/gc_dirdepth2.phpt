--TEST--
session GC cleans expired sessions with save_path dirdepth=2 (two subdir levels)
--EXTENSIONS--
session
--SKIPIF--
<?php include(__DIR__ . '/../skipif.inc'); ?>
--INI--
session.gc_probability=0
session.gc_maxlifetime=10
--FILE--
<?php
$base = __DIR__ . '/gc_dirdepth2_test';
@mkdir($base);
@mkdir("$base/a");
@mkdir("$base/a/b");

session_save_path("2;$base");

$stale_id   = 'abcdefghijklmnopqrstuvwx';
$stale_file = "$base/a/b/sess_$stale_id";
file_put_contents($stale_file, 'user|s:5:"alice";');
touch($stale_file, time() - 100);

session_id('ab000000000000000000000000');
session_start();
$result = session_gc();
session_destroy();

echo "session_gc() return value: ";
var_dump($result);

echo "expired file removed: ";
var_dump(!file_exists($stale_file));
?>
--CLEAN--
<?php
$base = __DIR__ . '/gc_dirdepth2_test';
@unlink("$base/a/b/sess_ab000000000000000000000000");
@rmdir("$base/a/b");
@rmdir("$base/a");
@rmdir($base);
?>
--EXPECT--
session_gc() return value: int(1)
expired file removed: bool(true)
