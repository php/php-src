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
include $dirname . 'utils.php';
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

echo $zip->locateName('entry1.txt') . "\n";
echo $zip->locateName('eNtry2.txt') . "\n";
echo $zip->locateName('eNtry2.txt', ZIPARCHIVE::FL_NOCASE) . "\n";
echo $zip->locateName('enTRy2d.txt', ZIPARCHIVE::FL_NOCASE|ZIPARCHIVE::FL_NODIR) . "\n";
$zip->close();

?>
--EXPECTF--
0
-1
1
2
