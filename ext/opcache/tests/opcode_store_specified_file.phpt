--TEST--
Compile php and store opcode to the specified file
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=1
opcache.optimization_level=-1
opcache.jit=0
opcache.file_cache={PWD}/opcode_store_specified_file_tmp
opcache.allow_direct_exec_opcode=1
--SKIPIF--
<?php require_once('skipif.inc');
$cache_dir = __DIR__ . DIRECTORY_SEPARATOR. 'opcode_store_specified_file_tmp';
if(!file_exists($cache_dir)) {
    mkdir($cache_dir);
}
?>
--FILE--
<?php

if(!isset($is_include)) {
    $file  = __FILE__;
    $ret = opcache_compile_file($file, __DIR__ .DIRECTORY_SEPARATOR. 'opcode_store_specified_file.bin.php');
    var_dump($ret);
    if(!$ret) {
        exit;
    }
    $is_include = 1;
    include_once __DIR__ . DIRECTORY_SEPARATOR. 'opcode_store_specified_file.bin.php';
    var_dump('inculde end');
    exit;
} else {
    function message() {
        var_dump('opcode include');
    }
    message();
}
?>
--CLEAN--
<?php
function rmdirr($dir_path) {
    $d = dir($dir_path);
    while(false !== ($f = $d->read())) {
        if($f === '.' || $f === '..') {
            continue;
        }
        $path = $dir_path. DIRECTORY_SEPARATOR. $f;
        if(is_dir($path)) {
        rmdirr($path);
        } else {
            unlink($path);
        }
    }
    rmdir($dir_path);
}
rmdirr(__DIR__.DIRECTORY_SEPARATOR.'opcode_store_specified_file_tmp');
unlink(__DIR__ .DIRECTORY_SEPARATOR. 'opcode_store_specified_file.bin.php');
?>
--EXPECT--
bool(true)
string(14) "opcode include"
string(11) "inculde end"
