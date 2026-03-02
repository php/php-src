--TEST--
Phar: tar-based phar, require_hash=1, no signature
--EXTENSIONS--
phar
--INI--
phar.readonly=1
phar.require_hash=0
--FILE--
<?php
ini_set('phar.require_hash', 1);
include __DIR__ . '/files/tarmaker.php.inc';
$fname = __DIR__ . '/require_hash.phar.tar';
$alias = 'phar://' . $fname;
$fname2 = __DIR__ . '/require_hash.tar';

$tar = new tarmaker($fname, 'none');
$tar->init();
$tar->addFile('tar_004.php', '<?php var_dump(__FILE__);');
$tar->addFile('internal/file/here', "hi there!\n");
$tar->addFile('.phar/stub.php', "__HALT_COMPILER();");
$tar->close();

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
--CLEAN--
<?php
@unlink(__DIR__ . '/require_hash.phar.tar');
@unlink(__DIR__ . '/require_hash.tar');
?>
--EXPECTF--
tar-based phar "%srequire_hash.phar.tar" does not have a signature
bool(false)
array(2) {
  ["hash"]=>
  string(32) "%s"
  ["hash_type"]=>
  string(3) "MD5"
}
