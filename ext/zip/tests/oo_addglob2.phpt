--TEST--
ZipArchive::addGlob() method with more compression and encryption
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
if (!method_exists('ZipArchive', 'setEncryptionName')) die('skip encrytion not supported');
if(!defined("GLOB_BRACE")) die ('skip requires GLOB_BRACE');
?>
--FILE--
<?php
$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';

$dirname = __DIR__ . '/__tmp_oo_addglob2/';
$file = $dirname . 'test.zip';

@mkdir($dirname);
copy(__FILE__, $dirname . 'foo.txt');
copy(__FILE__, $dirname . 'bar.txt');

$zip = new ZipArchive();
if (!$zip->open($file, ZipArchive::CREATE | ZipArchive::OVERWRITE)) {
        exit('failed');
}
$options = [
    'remove_all_path' => true,
];
if (!$zip->addGlob($dirname . 'foo.*', GLOB_BRACE, $options)) {
        echo "failed 1\n";
}

$options = [
    'remove_all_path' => true,
    'comp_method' => ZipArchive::CM_STORE,
    'comp_flags' => 5,
    'enc_method' => ZipArchive::EM_AES_256,
    'enc_password' => 'secret',
];
if (!$zip->addGlob($dirname . 'bar.*', GLOB_BRACE, $options)) {
        echo "failed 2\n";
}
if ($zip->status == ZIPARCHIVE::ER_OK) {
        $zip->close();

        $zip = new ZipArchive();
        $zip->open($file);
        for($i=0; $i<$zip->numFiles; $i++) {
            $sb = $zip->statIndex($i);
            echo "$i: " . $sb['name'] .
                ", comp=" . $sb['comp_method'] .
                ", enc="  . $sb['encryption_method'] . "\n";
        }
} else {
        echo "failed 3\n";
}
?>
--CLEAN--
<?php
$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
rmdir_rf(__DIR__ . '/__tmp_oo_addglob2/');
?>
--EXPECT--
0: foo.txt, comp=8, enc=0
1: bar.txt, comp=0, enc=259
