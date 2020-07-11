--TEST--
SPL: SplFileInfo::getExtension() basic test
--FILE--
<?php
$file = md5('SplFileInfo::getExtension');
$exts = array('.txt', '.extension', '..', '.', '');
foreach ($exts as $ext) {
    touch($file . $ext);
    $info = new SplFileInfo($file . $ext);
    var_dump($info->getExtension(), pathinfo($file . $ext, PATHINFO_EXTENSION));
}
?>
--CLEAN--
<?php
$file = md5('SplFileInfo::getExtension');
$exts = array('.txt', '.extension', '..', '.', '');
foreach ($exts as $ext) {
    unlink($file . $ext);
}
?>
--EXPECT--
string(3) "txt"
string(3) "txt"
string(9) "extension"
string(9) "extension"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
