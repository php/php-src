#!/usr/bin/env php
<?php declare(strict_types=1);

if($argc < 3) {
    echo "Usage: $argv[0] new original\n";
    exit(1);
}

if (!is_dir($argv[1]) ) {
    echo "$argv[1] is not a directory.\n";
    exit(1);
}

if (!is_dir($argv[2])) {
    echo "$argv[2] is not a directory.\n";
    exit(1);
}

$path1 = realpath($argv[1]);
$path2 = realpath($argv[2]);

$dir1 = glob($path1 . DIRECTORY_SEPARATOR. '*_arginfo.h');
$dir2 = glob($path2 . DIRECTORY_SEPARATOR. '*_arginfo.h');

$files1 = [];
$files2 = [];

array_walk($dir1, function (&$file) use (&$files1){
    $file = pathinfo($file);
    $files1[$file['basename']] = $file['dirname'];
});

array_walk($dir2, function (&$file) use (&$files2){
    $file = pathinfo($file);
    $files2[$file['basename']] = $file['dirname'];
});

foreach ($files1 as $filename => $path) {

    if (!isset($files2[$filename])) {
        echo "\e[0;31m${filename}\e[0m not exists in ${path2}";
        continue;
    }

    exec(sprintf('diff %s %s', $path.DIRECTORY_SEPARATOR.$filename, $path2.DIRECTORY_SEPARATOR.$filename), $output, $ret);
    if ($ret !== 0) {
        echo "\e[1;33m${filename}\e[0m not equal".PHP_EOL;
        array_walk($output, function (&$line) { $line = "\t".$line; });
        echo "\e[1;37m" . join(PHP_EOL, $output)."\e[0m".PHP_EOL;
    }
}

?>
