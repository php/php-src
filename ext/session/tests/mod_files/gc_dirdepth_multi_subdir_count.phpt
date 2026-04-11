--TEST--
session GC accumulates correct total count across multiple subdirs, including empty ones (dirdepth=1)
--EXTENSIONS--
session
--SKIPIF--
<?php include(__DIR__ . '/../skipif.inc'); ?>
--INI--
session.gc_probability=0
session.gc_maxlifetime=10
--FILE--
<?php
$base = __DIR__ . '/gc_multi_subdir_test';
@mkdir($base);
@mkdir("$base/a");
@mkdir("$base/b");
@mkdir("$base/c");
@mkdir("$base/d");   // empty subdir

session_save_path("1;$base");

$files = [
    "$base/a/sess_aexpired0000000000000000",
    "$base/b/sess_bexpired0000000000000000",
    "$base/c/sess_cexpired0000000000000000",
];
foreach ($files as $f) {
    file_put_contents($f, 'user|s:5:"alice";');
    touch($f, time() - 100);
}

session_id('a0000000000000000000000000');
session_start();
$result = session_gc();
session_destroy();

echo "session_gc() return value: ";
var_dump($result);

echo "all expired files removed: ";
var_dump(!file_exists($files[0]) && !file_exists($files[1]) && !file_exists($files[2]));
?>
--CLEAN--
<?php
$base = __DIR__ . '/gc_multi_subdir_test';
@unlink("$base/a/sess_a0000000000000000000000000");
@rmdir("$base/a");
@rmdir("$base/b");
@rmdir("$base/c");
@rmdir("$base/d");
@rmdir($base);
?>
--EXPECT--
session_gc() return value: int(3)
all expired files removed: bool(true)
