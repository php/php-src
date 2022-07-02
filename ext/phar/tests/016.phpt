--TEST--
Phar::mapPhar invalid file (gzipped file length is too short)
--EXTENSIONS--
phar
zlib
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";
// file length is too short

$files = array();
// "hi" gzdeflated
$files['a'] = array('cont'=>'a','comp'=> pack('H*', 'cbc80400'),'flags'=>0x00001000, 'ulen' => 1, 'clen' => 4);
$files['b'] = $files['a'];
$files['c'] = array('cont'=>'*');
$files['d'] = $files['a'];
include 'files/phar_test.inc';

var_dump(file_get_contents($pname . '/a'));
var_dump(file_get_contents($pname . '/b'));
var_dump(file_get_contents($pname . '/c'));
var_dump(file_get_contents($pname . '/d'));
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Warning: file_get_contents(phar://%s/a): Failed to open stream: phar error: internal corruption of phar "%s" (actual filesize mismatch on file "a") in %s on line %d
bool(false)

Warning: file_get_contents(phar://%s/b): Failed to open stream: phar error: internal corruption of phar "%s" (actual filesize mismatch on file "b") in %s on line %d
bool(false)
string(1) "*"

Warning: file_get_contents(phar://%s/d): Failed to open stream: phar error: internal corruption of phar "%s" (actual filesize mismatch on file "d") in %s on line %d
bool(false)
