--TEST--
Phar::loadPhar ignoring alias
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = '<?php include "' . $pname . '/a.php"; __HALT_COMPILER(); ?>';

$files = array();
$files['a.php']   = '<?php echo "This is a\n"; include \''.$pname.'/b.php\'; ?>';
$files['b.php']   = '<?php echo "This is b\n"; include \''.$pname.'/b/c.php\'; ?>';
$files['b/c.php'] = '<?php echo "This is b/c\n"; include \''.$pname.'/b/d.php\'; ?>';
$files['b/d.php'] = '<?php echo "This is b/d\n"; include \''.$pname.'/e.php\'; ?>';
$files['e.php']   = '<?php echo "This is e\n"; ?>';
$files['.phar/test'] = '<?php bad boy ?>';

include 'files/phar_test.inc';

Phar::loadPhar($fname);

require $pname . '/a.php';

$p = new Phar($fname);
var_dump(isset($p['.phar/test']));
try {
$p['.phar/test'];
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
?>
--EXPECT--
This is a
This is b
This is b/c
This is b/d
This is e
bool(false)
Cannot directly get any files or directories in magic ".phar" directory
===DONE===
