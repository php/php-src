--TEST--
ZipArchive::addGlob() method leaking after several calls when encryption is set.
--EXTENSIONS--
zip
--SKIPIF--
<?php
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
    'comp_method' => ZipArchive::CM_STORE,
    'comp_flags' => 5,
    'enc_method' => ZipArchive::EM_AES_256,
    'enc_password' => 'secret',
];
var_dump($zip->addGlob($dirname . 'bar.*', GLOB_BRACE, $options));
var_dump($zip->addGlob($dirname . 'bar.*', GLOB_BRACE, $options));
?>
--CLEAN--
<?php
$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
rmdir_rf(__DIR__ . '/__tmp_oo_addglob2/');
?>
--EXPECTF--
array(1) {
  [0]=>
  string(%d) "%s"
}
array(1) {
  [0]=>
  string(%d) "%s"
}
