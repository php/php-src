--TEST--
Phar: extracted
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
phar.extract_list="phar_test.phar=test"
--FILE--
<?php
var_dump(Phar::getExtractList());
ini_set('phar.extract_list', 'phar_test.phar='.dirname(__FILE__));
var_dump(Phar::getExtractList());

$pname = 'phar://phar_test.phar';

var_dump(file_get_contents($pname . '/files/extracted.inc'));

include $pname . '/files/extracted.inc';

?>
===DONE===
--EXPECTF--
array(1) {
  ["phar_test.phar"]=>
  string(4) "test"
}
array(1) {
  ["phar_test.phar"]=>
  string(%d) "%s"
}
string(%d) "<?php var_dump(__FILE__); ?>
"
string(%d) "%sextracted.inc"
===DONE===
