--TEST--
Phar: opendir test - no dir specified at all
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "<")) die("skip Unicode support required");
?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = b"<?php
Phar::mapPhar('hio');
var_dump(__FILE__);
var_dump(substr(__FILE__, 0, 4) != 'phar');
__HALT_COMPILER(); ?>";

$files = array();
$files['a'] = 'abc';
include 'files/phar_test.inc';

include $pname;
$dir = opendir('phar://hio');
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
unicode(%d) "%s017U.phar.php"
bool(true)

Warning: opendir(phar://hio): failed to open dir: phar error: no directory in "phar://hio", must have at least phar://hio/ for root directory (always use full path to a new phar)
phar url "phar://hio" is unknown in %s017U.php on line %d
