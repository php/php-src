--TEST--
Locate entries by name
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
include $dirname . 'utils.inc';
$file = $dirname . '__tmp_oo_rename.zip';

@unlink($file);

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
	exit('failed');
}

$zip->addFromString('entry1.txt', 'entry #1');
$zip->addFromString('entry2.txt', 'entry #2');
$zip->addFromString('dir/entry2d.txt', 'entry #2');

if (!$zip->status == ZIPARCHIVE::ER_OK) {
	echo "failed to write zip\n";
}
$zip->close();

if (!$zip->open($file)) {
	exit('failed');
}


var_dump($zip->locateName('entry1.txt'));
var_dump($zip->locateName('eNtry2.txt'));
var_dump($zip->locateName('eNtry2.txt', ZIPARCHIVE::FL_NOCASE));
var_dump($zip->locateName('enTRy2d.txt', ZIPARCHIVE::FL_NOCASE|ZIPARCHIVE::FL_NODIR));
$zip->close();

?>
--EXPECTF--
int(0)
bool(false)
int(1)
int(2)
