--TEST--
session GC deletes only expired sess_* files and leaves all other files untouched (dirdepth=1)
--EXTENSIONS--
session
--SKIPIF--
<?php include(__DIR__ . '/../skipif.inc'); ?>
--INI--
session.gc_probability=0
session.gc_maxlifetime=10
--FILE--
<?php
$base = __DIR__ . '/gc_selective_test';
@mkdir($base);
@mkdir("$base/a");

session_save_path("1;$base");

$expired = "$base/a/sess_aexpired0000000000000000";
$fresh   = "$base/a/sess_afresh000000000000000000";
$other   = "$base/a/other_file";

file_put_contents($expired, 'user|s:5:"alice";');
touch($expired, time() - 100);   // 100 s old > gc_maxlifetime=10 → deleted

file_put_contents($fresh, 'user|s:5:"alice";');
touch($fresh, time() - 1);       // 1 s old < gc_maxlifetime=10 → kept

file_put_contents($other, 'untouched');
touch($other, time() - 100);     // old but no sess_ prefix → kept

session_id('a0000000000000000000000000');  // first char 'a' → $base/a/
session_start();
$result = session_gc();   // int(1): exactly one deletion proves selectivity
session_destroy();

echo "session_gc() return value: ";
var_dump($result);

echo "expired sess_ file removed: ";
var_dump(!file_exists($expired));

echo "other file kept: ";
var_dump(file_exists($other));
?>
--CLEAN--
<?php
$base = __DIR__ . '/gc_selective_test';
@unlink("$base/a/sess_afresh000000000000000000");
@unlink("$base/a/sess_a0000000000000000000000000");
@unlink("$base/a/other_file");
@rmdir("$base/a");
@rmdir($base);
?>
--EXPECT--
session_gc() return value: int(1)
expired sess_ file removed: bool(true)
other file kept: bool(true)
