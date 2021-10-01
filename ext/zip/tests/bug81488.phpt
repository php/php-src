--TEST--
Bug #81488 (ext/zip doesn't extract files with special names)
--SKIPIF--
<?php
if (!extension_loaded("zip")) die("skip zip extension not available");
?>
--FILE--
<?php
$filenames = ["foo<bar1", "foo>bar2", "foo|bar3", "foo*bar4", "foo?bar5", "foo\"bar6", "foo:bar7"];
$archive = __DIR__ . "/bug81488.zip";
$dir = __DIR__ . "/bug81488";
mkdir($dir);

$zip = new ZipArchive();
$zip->open($archive, ZipArchive::CREATE|ZipArchive::OVERWRITE);
foreach ($filenames as $i => $filename) {
    $zip->addFromString($filename, "yada yada");
}
$zip->close();

$zip->open($archive);
foreach ($filenames as $filename) {
    var_dump($zip->extractTo($dir, $filename));
}
var_dump(scandir($dir));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
array(9) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(8) "foo_bar1"
  [3]=>
  string(8) "foo_bar2"
  [4]=>
  string(8) "foo_bar3"
  [5]=>
  string(8) "foo_bar4"
  [6]=>
  string(8) "foo_bar5"
  [7]=>
  string(8) "foo_bar6"
  [8]=>
  string(8) "foo_bar7"
}
--CLEAN--
<?php
$dir = __DIR__ . "/bug81488";
foreach (scandir($dir) as $filename) {
    if ($filename[0] !== ".") {
        @unlink("$dir/$filename");
    }
}
@rmdir($dir);
@unlink(__DIR__ . "/bug81488.zip");
?>
