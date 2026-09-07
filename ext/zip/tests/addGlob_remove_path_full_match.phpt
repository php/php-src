--TEST--
ZipArchive::addGlob() with remove_path equal to a matched path
--EXTENSIONS--
zip
--FILE--
<?php
$dirname = __DIR__ . '/addGlob_remove_path_full_match_dir';
@mkdir($dirname);
touch($dirname . '/foo.txt');
touch($dirname . '/bar.txt');

$zip = new ZipArchive();
$zip->open($dirname . '/tmp.zip', ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addGlob($dirname . '/*.txt', 0, ['remove_path' => $dirname . '/foo.txt']);

$names = [];
for ($i = 0; $i < $zip->numFiles; $i++) {
    $names[] = str_replace(__DIR__ . '/', '', $zip->getNameIndex($i));
}
sort($names);
var_dump($names);

$zip->close();

?>
--CLEAN--
<?php
$dirname = __DIR__ . '/addGlob_remove_path_full_match_dir';
unlink($dirname . '/tmp.zip');
unlink($dirname . '/foo.txt');
unlink($dirname . '/bar.txt');
rmdir($dirname);
?>
--EXPECT--
array(2) {
  [0]=>
  string(42) "addGlob_remove_path_full_match_dir/bar.txt"
  [1]=>
  string(42) "addGlob_remove_path_full_match_dir/foo.txt"
}
