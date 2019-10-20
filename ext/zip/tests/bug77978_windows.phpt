--TEST--
Bug #77978 Wrong relative path for :/ Windows tests
--SKIPIF--
<?php
if (!extension_loaded('zip')) die('skip');
if (substr(PHP_OS, 0, 3) != 'WIN') die('Skip Windows tests on Unix systems');
 ?>
--FILE--
<?php
$error_reporting = error_reporting();
error_reporting(E_ALL ^ E_WARNING); 
$file = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978.zip';
$target = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978';
@mkdir($target);
$pathList = [
    "dir/test:/filename1.txt",
    "dir/test:a/filename2.txt",
    "dir./test/filename3.txt",
    "dir../test/filename4.txt",
    "dir/test/filename5.txt",
    "../abc/filename6.txt",
    "abc/filename6.txt",
    "abc/filename7.txt",
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
    "dir/filename17.txt"
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

for ($i=0; $i < $zipReader->count(); $i++) { 
    $stat = $zipReader->statIndex( $i );
    $path=$stat['name'];
    $dir=dirname($path);
    
    try{
        $zipReader->extractTo($target, $path);
    } catch(Exception $e) {
        //nothing
    }
}
$zipReader->close();


$resultList = [
    "dir/test:/filename1.txt",
    "dir/test:a/filename2.txt",
    "dir./test/filename3.txt",
    "test/filename3.txt",
    "dir../test/filename4.txt",
    "test/filename4.txt",
    "dir/test/filename5.txt",
    "../abc/filename6.txt",
    "abc/filename6.txt",
    "abc/filename7.txt",
    "/abc/filename8.txt",
    "abc/filename9.txt",
    ":abc/filename10.txt",
    "ab:c/filename11.txt",
    "abc:/filename12.txt",
    "abc/.filename13.txt",
    "abc/..filename14.txt",
    "abc/../filename15.txt",
    "filename15.txt",
    "abc/../../filename16.txt",
    "filename16.txt",
    "abc/../../dir/filename17.txt",
    "dir/filename17.txt",
    "abc/./filename18.txt",
    "abc/file:name19.txt",
    "abc/file.name20.txt",
    "abc//filename21.txt",
    "C:abc/filename22.txt",
    "abc/filename22.txt",
    "C:\abc/filename23.txt",
    "abc/filename23.txt",
    "C:/abc/filename24.txt",
    "abc/filename24.txt"
];

foreach ($resultList as $path) {
    $result = file_exists($target . DIRECTORY_SEPARATOR . $path) ? 'found' : 'not found';
    printf("%s %s%s", $path, $result, PHP_EOL);
}

$error_reporting = $error_reporting;
?>

--CLEAN--
<?php
$file = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978.zip';
$target = __DIR__ . DIRECTORY_SEPARATOR . 'bug77978';
if(file_exists($file)){
    unlink($file);
}

if(file_exists($target)){
    $it = new RecursiveDirectoryIterator($target, RecursiveDirectoryIterator::SKIP_DOTS);
    $files = new RecursiveIteratorIterator($it,
                 RecursiveIteratorIterator::CHILD_FIRST);
    foreach($files as $file) {
        if ($file->isDir()){
            rmdir($file->getRealPath());
        } else {
            unlink($file->getRealPath());
        }
    }
    rmdir($target);
}
?>
--EXPECT--
dir/test:/filename1.txt not found
dir/test:a/filename2.txt not found
dir./test/filename3.txt not found
test/filename3.txt found
dir../test/filename4.txt not found
test/filename4.txt found
dir/test/filename5.txt found
../abc/filename6.txt not found
abc/filename6.txt found
abc/filename7.txt found
/abc/filename8.txt found
abc/filename9.txt found
:abc/filename10.txt not found
ab:c/filename11.txt not found
abc:/filename12.txt not found
abc/.filename13.txt found
abc/..filename14.txt found
abc/../filename15.txt found
filename15.txt found
abc/../../filename16.txt not found
filename16.txt found
abc/../../dir/filename17.txt not found
dir/filename17.txt found
abc/./filename18.txt found
abc/file:name19.txt not found
abc/file.name20.txt found
abc//filename21.txt found
C:abc/filename22.txt not found
abc/filename22.txt found
C:\abc/filename23.txt not found
abc/filename23.txt found
C:/abc/filename24.txt not found
abc/filename24.txt found
