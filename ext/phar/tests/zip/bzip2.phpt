--TEST--
Phar: process bzip2-compressed zip entry
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not available"); ?>
--FILE--
<?php
try {
	$a = new PharData(dirname(__FILE__) . '/files/bzip2.zip');
	foreach ($a as $entry => $file) {
		echo $file->getContent();
	}
	$a = new Phar(dirname(__FILE__) . '/files/bz2_alias.phar.zip');
	var_dump($a->getAlias());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECT--
<?php
include dirname(__FILE__) . '/corrupt_zipmaker.php.inc';
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii');
$a->addFile('hi2', null, 'hii2', null, null, 'encrypt', 'encrypt');
$a->writeZip(dirname(__FILE__) . '/encrypted.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii');
$a->addFile('', null, 'stdin');
$a->writeZip(dirname(__FILE__) . '/stdin.zip');
$a = new corrupt_zipmaker;
$a->addFile('hii', null, 'hii', null, null, 'filename_len', 'filename_len');
$a->addFile('hi', null, 'hii');
$a->writeZip(dirname(__FILE__) . '/truncfilename.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress');
$a->writeZip(dirname(__FILE__) . '/compress_unsup1.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 2);
$a->writeZip(dirname(__FILE__) . '/compress_unsup2.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 3);
$a->writeZip(dirname(__FILE__) . '/compress_unsup3.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 4);
$a->writeZip(dirname(__FILE__) . '/compress_unsup4.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 5);
$a->writeZip(dirname(__FILE__) . '/compress_unsup5.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 6);
$a->writeZip(dirname(__FILE__) . '/compress_unsup6.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 7);
$a->writeZip(dirname(__FILE__) . '/compress_unsup7.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 9);
$a->writeZip(dirname(__FILE__) . '/compress_unsup9.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 10);
$a->writeZip(dirname(__FILE__) . '/compress_unsup10.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 14);
$a->writeZip(dirname(__FILE__) . '/compress_unsup14.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 18);
$a->writeZip(dirname(__FILE__) . '/compress_unsup18.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 19);
$a->writeZip(dirname(__FILE__) . '/compress_unsup19.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 97);
$a->writeZip(dirname(__FILE__) . '/compress_unsup97.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 98);
$a->writeZip(dirname(__FILE__) . '/compress_unsup98.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, 'compress', 'compress', 11);
$a->writeZip(dirname(__FILE__) . '/compress_unsupunknown.zip');
?>
string(175) "hitheresuperlongzipistoostupidtodowhatIsaysoIhavetousethisridiculouslylongaliasbecauseitisstupiddidImentionthatalreadythemadnessdoesnotstopIhateinfozipIhateinfozipIhateinfozip"
===DONE===
