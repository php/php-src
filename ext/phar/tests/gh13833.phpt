--TEST--
GH-13833 (Applying zero offset to null pointer in zend_hash.c)
--INI--
phar.require_hash=0
phar.readonly=0
--SKIPIF--
<?php if (defined("PHP_WINDOWS_VERSION_MAJOR")) die("skip Windows results in a different error"); ?>
--FILE--
<?php
$fname = __DIR__ . '/gh13833.phar';
$pname = 'phar://' . $fname;
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";
$files = array();
$files['a'] = 'a';
include 'files/phar_test.inc';
include $fname;

$file = "<?php __HALT_COMPILER(); ?>";
$files['a'] = array('cont' => 'a');
include 'files/phar_test.inc';

$phar = new Phar($fname);
$phar->setMetadata((object) ['my' => 'friend']);
// NOTE: Phar will use the cached value of metadata if setMetaData was called on that Phar path before.
// Save the writes to the phar and use a different file path.
$fname_new = "$fname.copy.phar";
copy($fname, $fname_new);
try {
  new Phar($fname_new);
} catch (UnexpectedValueException $e) {
  echo $e->getMessage(), "\n";
}
?>
--EXTENSIONS--
phar
--CLEAN--
<?php
unlink(__DIR__ . '/gh13833.phar');
unlink(__DIR__ . '/gh13833.phar.copy.phar');
?>
--CREDITS--
Yuancheng Jiang <yuancheng@comp.nus.edu.sg>
Felix De Vliegher <felix.devliegher@gmail.com>
--EXPECTF--
internal corruption of phar "%sgh13833.phar.copy.phar" (trying to read past buffer end)
