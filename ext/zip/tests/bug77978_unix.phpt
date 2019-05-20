--TEST--
Bug #77978 Wrong relative path for :/ unix tests
--SKIPIF--
<?php
if (!extension_loaded('zip')) die('skip');
if (substr(PHP_OS, 0, 3) == 'WIN') die('Skip Unix tests on Windows');
 ?>
--FILE--
<?php

$file = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978_unix.zip';
$target = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978_unix';
@mkdir($target);

$pathList = [
    "dir/test:/filename1.txt",
    "dir/test:a/filename2.txt",
    "dir./test/filename3.txt",
    "dir../test/filename4.txt",
    "dir/test/filename5.txt",
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
    "abc/file:name19.txt",
    "abc/file.name20.txt",
    "abc//filename21.txt",
    "C:abc/filename22.txt",
    "C:\abc/filename23.txt",
    "C:/abc/filename24.txt",
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
    "abc/filename6.txt",
    "filename16.txt",
    "dir/filename17.txt",
];

foreach($pathListAdditional as $path) {
    $file = $target . DIRECTORY_SEPARATOR . $path;
    $result = file_exists($file) ? 'found' : 'not found';
    printf("%s \t%s%s", $path, $result, PHP_EOL);
}

?>
--CLEAN--
<?php
$file = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978_unix.zip';
$target = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978_unix';
unlink($file);

$pathList = [
    "dir/test:/filename1.txt",
    "dir/test:a/filename2.txt",
    "dir./test/filename3.txt",
    "dir../test/filename4.txt",
    "dir/test/filename5.txt",
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
    "abc/file:name19.txt",
    "abc/file.name20.txt",
    "abc//filename21.txt",
    "C:abc/filename22.txt",
    "C:\abc/filename23.txt",
    "C:/abc/filename24.txt",
    "abc/filename6.txt",
    "filename16.txt",
    "dir/filename17.txt",
];
foreach($pathList as $path) {
    if(file_exists($target . DIRECTORY_SEPARATOR . $path)) {
        unlink($target . DIRECTORY_SEPARATOR . $path);
    }
}
rmdir($target);
?>
--EXPECT--
dir/test:/filename1.txt 	found
dir/test:a/filename2.txt 	found
dir./test/filename3.txt 	found
dir../test/filename4.txt 	found
dir/test/filename5.txt 	found
../abc/filename6.txt 	not found
./abc/filename7.txt 	found
/abc/filename8.txt 	found
abc/filename9.txt 	found
:abc/filename10.txt 	found
ab:c/filename11.txt 	found
abc:/filename12.txt 	found
abc/.filename13.txt 	found
abc/..filename14.txt 	found
abc/../filename15.txt 	found
abc/../../filename16.txt 	not found
abc/../../dir/filename17.txt 	not found
abc/./filename18.txt 	found
abc/file:name19.txt 	found
abc/file.name20.txt 	found
abc//filename21.txt 	found
C:abc/filename22.txt 	found
C:\abc/filename23.txt 	found
C:/abc/filename24.txt 	found
abc/filename6.txt 	found
filename16.txt 	found
dir/filename17.txt 	found