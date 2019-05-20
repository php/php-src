--TEST--
Bug #77978 Wrong relative path for :/ windows tests
--SKIPIF--
<?php
if (!extension_loaded('zip')) die('skip');
if (substr(PHP_OS, 0, 3) != 'WIN') die('Skip Windows tests on Unix systems');
 ?>
--FILE--
<?php

$file = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978_windows.zip';
$target = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978_windows';
@mkdir($target);

$pathList = [
    "abc/test:/filename1.txt",
    "abc/test:a/filename2.txt",
    "abc./test/filename3.txt",
    "abc../test/filename4.txt",
    "abc/test/filename5.txt",
    "../abc/filename6.txt",
    "./abc/filename7.txt",
    "/abc/filename8.txt",
    "abc/filename9.txt",
    ":abc/filename10.txt",
    "ab:c/filename11.txt",
    "abc:/filename12.txt",
    "abc/.filename13.txt",
    "abc/..filename14.txt",
    "abc/../filename15.txt",
    "abc/../../filename16.txt",
    "abc/../../dir/filename17.txt",
    "abc/./filename18.txt",
    "abc/file.name19.txt",
    "abc//filename20.txt",
    "C:abc/filename21.txt",
    "C:\abc/filename22.txt",
    "C:/abc/filename23.txt",
];

$zipWriter = new ZipArchive();
$zipWriter->open($file, ZIPARCHIVE::CREATE);
foreach($pathList as $path) {
    $zipWriter->addFromString($path, "contents");
}
$zipWriter->close();

$zipReader = new ZipArchive();

$i = 0;
while($zipReader->open($file) !== true && $i < 30) {
    ++$i;
    if($i == 30) {
        die("Can't open zip file {$file} for read.");
    }
    sleep(1);
}

foreach($pathList as $path) {
    $zipReader->extractTo($target, $path);
    $result = file_exists($target . DIRECTORY_SEPARATOR . $path) ? 'found' : 'not found';
    printf("%s \t%s%s", $path, $result, PHP_EOL);
}
$zipReader->close();

unlink($file);

$pathListAdditional = [
    "test/filename4.txt",
    "abc/filename6.txt",
    "filename16.txt",
    "dir/filename17.txt",
    "abc/filename22.txt",
    "abc/filename23.txt",
];

foreach($pathListAdditional as $path) {
    $file = $target . DIRECTORY_SEPARATOR . $path;
    $result = file_exists($file) ? 'found' : 'not found';
    printf("%s \t%s%s", $path, $result, PHP_EOL);
}

?>
--CLEAN--
<?php
$file = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978_windows.zip';
$target = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978_windows';
unlink($file);

$pathList = [
    "abc./test/filename3.txt",
    "test/filename4.txt",
    "abc/test/filename5.txt",
    "../abc/filename6.txt",
    "./abc/filename7.txt",
    "/abc/filename8.txt",
    "abc/filename9.txt",
    ":abc/filename10.txt",
    "abc/.filename13.txt",
    "abc/..filename14.txt",
    "abc/../filename15.txt",
    "filename16.txt",
    "dir/filename17.txt",
    "abc/./filename18.txt",
    "abc/file.name19.txt",
    "abc//filename20.txt",
    "abc/filename21.txt",
    "abc/filename22.txt",
    "abc/filename23.txt",
    "abc/filename6.txt",
    "filename16.txt",
    "abc/filename17.txt",
];
foreach($pathList as $path) {
    if(file_exists($target . DIRECTORY_SEPARATOR . $path)) {
        unlink($target . DIRECTORY_SEPARATOR . $path);
    }
}
rmdir($target);
--EXPECTF--
Warning: ZipArchive::extractTo(): No such file or directory in %s on line %d
abc/test:/filename1.txt 	not found

Warning: ZipArchive::extractTo(): Not a directory in %s on line %d
abc/test:a/filename2.txt 	not found
abc./test/filename3.txt 	found
abc../test/filename4.txt 	not found
abc/test/filename5.txt 	found
../abc/filename6.txt 	not found
./abc/filename7.txt 	found
/abc/filename8.txt 	found
abc/filename9.txt 	found

Warning: ZipArchive::extractTo(): No such file or directory in %s on line %d
:abc/filename10.txt 	not found

Warning: ZipArchive::extractTo(): Not a directory in %s on line %d
ab:c/filename11.txt 	not found

Warning: ZipArchive::extractTo(): No such file or directory in %s on line %d
abc:/filename12.txt 	not found
abc/.filename13.txt 	found
abc/..filename14.txt 	found
abc/../filename15.txt 	found
abc/../../filename16.txt 	not found
abc/../../dir/filename17.txt 	not found
abc/./filename18.txt 	found
abc/file.name19.txt 	found
abc//filename20.txt 	found
C:abc/filename21.txt 	not found
C:\abc/filename22.txt 	not found
C:/abc/filename23.txt 	not found
test/filename4.txt 	found
abc/filename6.txt 	found
filename16.txt 	found
dir/filename17.txt 	found
abc/filename22.txt 	found
abc/filename23.txt 	found