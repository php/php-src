--TEST--
Test that ZipArchive properties are read-only
--EXTENSIONS--
zip
--FILE--
<?php

$zip = new ZipArchive();

try {
    $zip->lastId = 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $zip->lastId += 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($zip->lastId);

try {
    $zip->status = 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($zip->status);

try {
    $zip->statusSys = 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($zip->statusSys);

try {
    $zip->numFiles = 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($zip->numFiles);

try {
    $zip->filename = "a";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($zip->filename);

try {
    $zip->comment = "a";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($zip->comment);

?>
--EXPECT--
Cannot write read-only property ZipArchive::$lastId
Cannot write read-only property ZipArchive::$lastId
int(-1)
Cannot write read-only property ZipArchive::$status
int(0)
Cannot write read-only property ZipArchive::$statusSys
int(0)
Cannot write read-only property ZipArchive::$numFiles
int(0)
Cannot write read-only property ZipArchive::$filename
string(0) ""
Cannot write read-only property ZipArchive::$comment
string(0) ""
