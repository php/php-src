--TEST--
php-users@php.gr.jp #16242
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
?>
--FILE--
<?php
$ini = <<< HERE
mbstring.language=Japanese
mbstring.internal_encoding=UTF-8

HERE;

$tmpfile = tempnam(dirname(__FILE__), "t");
$fp = fopen($tmpfile, 'w');
fwrite($fp, $ini);
fclose($fp);

$cmd = 'echo "<?php ';
$cmd .= "var_dump(ini_get('mbstring.language'));";
$cmd .= "var_dump(ini_get('mbstring.internal_encoding'));";
$cmd .= 'var_dump(mb_internal_encoding());';
$cmd .= ' ?>" |'.getenv("TEST_PHP_EXECUTABLE").' -c '.$tmpfile;

system($cmd);

unlink($tmpfile);
?>
--EXPECT--
string(8) "Japanese"
string(5) "UTF-8"
string(5) "UTF-8"
