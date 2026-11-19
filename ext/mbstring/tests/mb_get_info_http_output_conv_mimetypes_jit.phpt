--TEST--
mb_get_info() returning the mbstring.http_output_conv_mimetypes INI value under function JIT
--EXTENSIONS--
mbstring
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--ENV--
MIME_A=text/plain
FILLER=AAAAAAAAAA
--FILE--
<?php
ini_set('mbstring.http_output_conv_mimetypes', getenv('MIME_A'));
mb_get_info('http_output_conv_mimetypes');
$filler = getenv('FILLER');
echo "ini_get(): ";
var_dump(ini_get('mbstring.http_output_conv_mimetypes'));
?>
--EXPECT--
ini_get(): string(10) "text/plain"
