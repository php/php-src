--TEST--
session GC with save_path dirdepth does not follow symlinked subdirectories
--EXTENSIONS--
session
--SKIPIF--
<?php
include(__DIR__ . '/../skipif.inc');
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
?>
--INI--
session.gc_probability=0
session.use_strict_mode=0
--FILE--
<?php
$base = __DIR__ . '/gc_dirdepth_symlink';
$outside = "$base-outside";
@mkdir("$base/b", 0777, true);
@mkdir($outside);
symlink($outside, "$base/a");
touch("$outside/sess_a", time() - 2000);

session_save_path("1;$base");
session_id('b0');
session_start();
var_dump(session_gc(), file_exists("$outside/sess_a"));
?>
--CLEAN--
<?php
$base = __DIR__ . '/gc_dirdepth_symlink';
@unlink("$base/b/sess_b0");
@unlink("$base/a");
@rmdir("$base/b");
@rmdir($base);
@unlink("$base-outside/sess_a");
@rmdir("$base-outside");
?>
--EXPECT--
int(0)
bool(true)
