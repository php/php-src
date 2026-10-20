--TEST--
Bug #61470 (session_regenerate_id() does not create session file)
--EXTENSIONS--
session
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
ob_start();
mkdir(__DIR__ . '/bug61470');
ini_set('session.save_path', __DIR__ . '/bug61470');
$path = ini_get('session.save_path') . '/sess_';
session_start();
// starts session & creates and locks file
$file1 = $path . session_id();
var_dump(is_file($file1));

session_regenerate_id();
// starts new session, but file is not create!
$file2 = $path . session_id();
var_dump(is_file($file2));
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/bug61470/sess_*') as $file) {
    unlink($file);
}
rmdir(__DIR__ . '/bug61470');
?>
--EXPECT--
bool(true)
bool(true)
