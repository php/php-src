--TEST--
revalidate_path 01: OPCache must cache only resolved real paths when revalidate_path is set
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.revalidate_path=1
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php
$dir = __DIR__;
$dir1 = "$dir/test1";
$dir2 = "$dir/test2";
$link = "$dir/test";
$file1 = "$dir1/index.php";
$file2 = "$dir2/index.php";
$main = "$dir/main.php";
@mkdir($dir1);
@mkdir($dir2);
@file_put_contents($main,  '<?php include(\'' . $link .'/index.php\');');
@file_put_contents($file1, "TEST 1\n");
@file_put_contents($file2, "TEST 2\n");
while (filemtime($file1) != filemtime($file2)) {
    touch($file1);
    touch($file2);
}
if (substr(PHP_OS, 0, 3) == 'WIN') {
    @rmdir($link);
    $ln = str_replace('/', '\\', $link);
    $d1 = realpath($dir1);
    `mklink /j $ln $d1`;
} else {
    @unlink($link);
    @symlink($dir1, $link);
}

include "php_cli_server.inc";
//php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.revalidate_path=1');
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.revalidate_path=1 -d opcache.file_update_protection=0 -d realpath_cache_size=0');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/main.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/main.php');
if (substr(PHP_OS, 0, 3) == 'WIN') {
    @rmdir($link);
    $ln = str_replace('/', '\\', $link);
    $d2 = realpath($dir2);
    `mklink /j $ln $d2`;
} else {
    @unlink($link);
    @symlink($dir2, $link);
}
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/main.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/main.php');
?>
--CLEAN--
<?php
$dir = __DIR__;
$dir1 = "$dir/test1";
$dir2 = "$dir/test2";
$link = "$dir/test";
$file1 = "$dir1/index.php";
$file2 = "$dir2/index.php";
$main = "$dir/main.php";
@unlink($main);
if (substr(PHP_OS, 0, 3) == 'WIN') {
	@rmdir($link);
} else {
	@unlink($link);
}
@unlink($file1);
@unlink($file2);
@rmdir($dir1);
@rmdir($dir2);
?>
--EXPECT--
TEST 1
TEST 1
TEST 2
TEST 2
