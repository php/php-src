--TEST--
stream stat
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
function cleanup() { unlink(dirname(__FILE__) . '/008_phar.php'); }
register_shutdown_function('cleanup');
$file = "<?php
PHP_Archive::mapPhar(5, 'hio', false);
__HALT_COMPILER(); ?>";
$contents = 'abcdefg';
$manifest = pack('V', 1) . 'a' . pack('VVVV', strlen($contents), time(), 0, 8 + strlen($contents));
$file .= pack('VV', strlen($manifest) + 4, 1) .
	 $manifest .
	 pack('VV', crc32($contents), strlen($contents)) . $contents;
file_put_contents(dirname(__FILE__) . '/008_phar.php', $file);
include dirname(__FILE__) . '/008_phar.php';
$fp = fopen('phar://hio/a', 'r');
var_dump(ftell($fp));
echo 'fseek($fp, 1)';var_dump(fseek($fp, 1));
var_dump(ftell($fp));
echo 'fseek($fp, 1, SEEK_CUR)';var_dump(fseek($fp, 1, SEEK_CUR));
var_dump(ftell($fp));
echo 'fseek($fp, -1, SEEK_CUR)';var_dump(fseek($fp, -1, SEEK_CUR));
var_dump(ftell($fp));
echo 'fseek($fp, -1, SEEK_END)';var_dump(fseek($fp, -1, SEEK_END));
var_dump(ftell($fp));
echo 'fseek($fp, -8, SEEK_END)';var_dump(fseek($fp, -8, SEEK_END));
var_dump(ftell($fp));
echo 'fseek($fp, -7, SEEK_END)';var_dump(fseek($fp, -7, SEEK_END));
var_dump(ftell($fp));
echo 'fseek($fp, 0, SEEK_END)';var_dump(fseek($fp, 0, SEEK_END));
var_dump(ftell($fp));
echo 'fseek($fp, 1, SEEK_END)';var_dump(fseek($fp, 1, SEEK_END));
var_dump(ftell($fp));
echo 'fseek($fp, -8, SEEK_END)';var_dump(fseek($fp, -8, SEEK_END));
var_dump(ftell($fp));
echo 'fseek($fp, 6)';var_dump(fseek($fp, 6));
var_dump(ftell($fp));
echo 'fseek($fp, 8)';var_dump(fseek($fp, 8));
var_dump(ftell($fp));
echo 'fseek($fp, -1)';var_dump(fseek($fp, -1));
var_dump(ftell($fp));
echo "next\n";
fseek($fp, 4);
var_dump(ftell($fp));
echo 'fseek($fp, -5, SEEK_CUR)';var_dump(fseek($fp, -5, SEEK_CUR));
var_dump(ftell($fp));
fseek($fp, 4);
var_dump(ftell($fp));
echo 'fseek($fp, 5, SEEK_CUR)';var_dump(fseek($fp, 5, SEEK_CUR));
var_dump(ftell($fp));
fseek($fp, 4);
var_dump(ftell($fp));
echo 'fseek($fp, -4, SEEK_CUR)';var_dump(fseek($fp, -4, SEEK_CUR));
var_dump(ftell($fp));
fseek($fp, 4);
var_dump(ftell($fp));
echo 'fseek($fp, 3, SEEK_CUR)';var_dump(fseek($fp, 3, SEEK_CUR));
var_dump(ftell($fp));
fclose($fp);
?>
--EXPECT--
int(0)
fseek($fp, 1)int(0)
int(1)
fseek($fp, 1, SEEK_CUR)int(0)
int(2)
fseek($fp, -1, SEEK_CUR)int(0)
int(1)
fseek($fp, -1, SEEK_END)int(0)
int(6)
fseek($fp, -8, SEEK_END)int(-1)
bool(false)
fseek($fp, -7, SEEK_END)int(0)
int(0)
fseek($fp, 0, SEEK_END)int(0)
int(7)
fseek($fp, 1, SEEK_END)int(-1)
bool(false)
fseek($fp, -8, SEEK_END)int(-1)
bool(false)
fseek($fp, 6)int(0)
int(6)
fseek($fp, 8)int(-1)
bool(false)
fseek($fp, -1)int(-1)
bool(false)
next
int(4)
fseek($fp, -5, SEEK_CUR)int(-1)
bool(false)
int(4)
fseek($fp, 5, SEEK_CUR)int(-1)
bool(false)
int(4)
fseek($fp, -4, SEEK_CUR)int(0)
int(0)
int(4)
fseek($fp, 3, SEEK_CUR)int(0)
int(7)










