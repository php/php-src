--TEST--
Delete entries
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$dirname = __DIR__ . '/';
$file = $dirname . 'oo_delete.zip';
if (file_exists($file)) {
	unlink($file);
}

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
	exit('failed');
}
$zip->addFromString('entry1.txt', 'entry #1');
$zip->addFromString('entry2.txt', 'entry #2');
$zip->addFromString('dir/entry2.txt', 'entry #2');

if ($zip->status == ZIPARCHIVE::ER_OK) {
	$zip->close();
	echo "ok\n";
} else {
	var_dump($zip);
	echo "failed\n";
}

if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
	exit('failed');
}

if ($zip->deleteIndex(0)) {
	echo "ok\n";
}

if ($zip->deleteName('entry2.txt')) {
	echo "ok\n";
} else {
	echo "failed 3\n";
}

if ($zip->deleteName('dir/entry2.txt')) {
	echo "ok\n";
} else {
	echo "failed 3\n";
}

if (!$zip->deleteIndex(123)) {
	echo "ok\n";
} else {
	print_r($zip);
	echo "failed\n";
}


$sb = $zip->statIndex(0);
var_dump($sb);
$sb = $zip->statIndex(1);
var_dump($sb);
$sb = $zip->statIndex(2);
var_dump($sb);
// suppress irrelevant error message:
@$zip->close();
unset($zip);

if (file_exists($file)) {
	unlink($file);
}
?>
--EXPECT--
ok
ok
ok
ok
ok
bool(false)
bool(false)
bool(false)
