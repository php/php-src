--TEST--
Phar::buildFromIterator() RegexIterator(RecursiveIteratorIterator), SplFileInfo as current
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
try {
	chdir(__DIR__);
	$phar = new Phar(__DIR__ . '/buildfromiterator10.phar');
	$dir = new RecursiveDirectoryIterator('.');
	$iter = new RecursiveIteratorIterator($dir);
	$a = $phar->buildFromIterator(new RegexIterator($iter, '/_\d{3}\.phpt$/'), __DIR__ . DIRECTORY_SEPARATOR);
	asort($a);
	var_dump($a);
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/buildfromiterator10.phar');
__HALT_COMPILER();
?>
--EXPECTF--
array(34) {
  ["phar_ctx_001.phpt"]=>
  string(%d) "%sphar_ctx_001.phpt"
  ["phar_get_supported_signatures_002.phpt"]=>
  string(%d) "%sphar_get_supported_signatures_002.phpt"
  ["phar_oo_001.phpt"]=>
  string(%d) "%sphar_oo_001.phpt"
  ["phar_oo_002.phpt"]=>
  string(%d) "%sphar_oo_002.phpt"
  ["phar_oo_003.phpt"]=>
  string(%d) "%sphar_oo_003.phpt"
  ["phar_oo_004.phpt"]=>
  string(%d) "%sphar_oo_004.phpt"
  ["phar_oo_005.phpt"]=>
  string(%d) "%sphar_oo_005.phpt"
  ["phar_oo_006.phpt"]=>
  string(%d) "%sphar_oo_006.phpt"
  ["phar_oo_007.phpt"]=>
  string(%d) "%sphar_oo_007.phpt"
  ["phar_oo_008.phpt"]=>
  string(%d) "%sphar_oo_008.phpt"
  ["phar_oo_009.phpt"]=>
  string(%d) "%sphar_oo_009.phpt"
  ["phar_oo_010.phpt"]=>
  string(%d) "%sphar_oo_010.phpt"
  ["phar_oo_011.phpt"]=>
  string(%d) "%sphar_oo_011.phpt"
  ["phar_oo_012.phpt"]=>
  string(%d) "%sphar_oo_012.phpt"
  ["phar_oo_compressed_001.phpt"]=>
  string(%d) "%sphar_oo_compressed_001.phpt"
  ["phar_oo_compressed_002.phpt"]=>
  string(%d) "%sphar_oo_compressed_002.phpt"
  ["phpinfo_001.phpt"]=>
  string(%d) "%sphpinfo_001.phpt"
  ["phpinfo_002.phpt"]=>
  string(%d) "%sphpinfo_002.phpt"
  ["phpinfo_003.phpt"]=>
  string(%d) "%sphpinfo_003.phpt"
  ["phpinfo_004.phpt"]=>
  string(%d) "%sphpinfo_004.phpt"
  ["tar/tar_001.phpt"]=>
  string(%d) "%star%ctar_001.phpt"
  ["tar/tar_002.phpt"]=>
  string(%d) "%star%ctar_002.phpt"
  ["tar/tar_003.phpt"]=>
  string(%d) "%star%ctar_003.phpt"
  ["tar/tar_004.phpt"]=>
  string(%d) "%star%ctar_004.phpt"
  ["zip/corrupt_001.phpt"]=>
  string(%d) "%szip%ccorrupt_001.phpt"
  ["zip/corrupt_002.phpt"]=>
  string(%d) "%szip%ccorrupt_002.phpt"
  ["zip/corrupt_003.phpt"]=>
  string(%d) "%szip%ccorrupt_003.phpt"
  ["zip/corrupt_004.phpt"]=>
  string(%d) "%szip%ccorrupt_004.phpt"
  ["zip/corrupt_005.phpt"]=>
  string(%d) "%szip%ccorrupt_005.phpt"
  ["zip/corrupt_006.phpt"]=>
  string(%d) "%szip%ccorrupt_006.phpt"
  ["zip/corrupt_007.phpt"]=>
  string(%d) "%szip%ccorrupt_007.phpt"
  ["zip/corrupt_008.phpt"]=>
  string(%d) "%szip%ccorrupt_008.phpt"
  ["zip/corrupt_009.phpt"]=>
  string(%d) "%szip%ccorrupt_009.phpt"
  ["zip/corrupt_010.phpt"]=>
  string(%d) "%szip%ccorrupt_010.phpt"
}
===DONE===
