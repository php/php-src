--TEST--
Bug #66431 Special Character via COM Interface (CP_UTF8), Scripting.FileSystemObject
--SKIPIF--
<?php
if (!extension_loaded("com_dotnet")){ echo "skip COM/.Net support not present"; }
?>
--FILE--
<?php

$text= "Xin chào cộng đồng PHP";
$fpath = str_replace("/", "\\", __DIR__ . "/bug66431.txt");

$fso = new COM("Scripting.FileSystemObject");
$fh = $fso->OpenTextFile($fpath, 2, true);
$fh->Write($text);
$fh->Close();

$check_text = file_get_contents($fpath);

$result = ($check_text == $text);

var_dump($result);

if (!$result) {
    echo "Expected: '$check_text'\n";
    echo "Have: '$text'\n";
}

?>
--CLEAN--
<?php

$fpath = str_replace("/", "\\", __DIR__ . "/bug66431.txt");

if (file_exists($fpath)) {
    unlink($fpath);
}
?>
--EXPECT--
bool(true)
