--TEST--
Phar: zip-based phar, require_hash=1, no signature
--EXTENSIONS--
phar
zip
--INI--
phar.readonly=1
phar.require_hash=0
--FILE--
<?php
ini_set('phar.require_hash', 1);
include __DIR__ . '/files/zipmaker.php.inc';
$fname = __DIR__ . '/require_hash.phar.zip';
$alias = 'phar://' . $fname;
$fname2 = __DIR__ . '/require_hash.zip';

$zip = new zipmaker($fname);
$zip->init();
$zip->addFile('zip_001.php', '<?php var_dump(__FILE__);');
$zip->addFile('internal/file/here', "hi there!\n");
$zip->addFile('.phar/stub.php', "__HALT_COMPILER();");
$zip->close();

try {
	$phar = new Phar($fname);
	var_dump($phar->getStub());
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}
ini_set('phar.require_hash', 0);
try {
	$phar = new PharData($fname2);
	$phar['file'] = 'hi';
	var_dump($phar->getSignature());
	$phar->setSignatureAlgorithm(Phar::MD5);
	var_dump($phar->getSignature());
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}

?>
===DONE===
--CLEAN--
<?php
@unlink(__DIR__ . '/require_hash.phar.zip');
@unlink(__DIR__ . '/require_hash.zip');
?>
--EXPECTF--
phar error: signature is missing in zip-based phar "%srequire_hash.phar.zip"
bool(false)
array(2) {
  ["hash"]=>
  string(32) "%s"
  ["hash_type"]=>
  string(3) "MD5"
}
===DONE===
