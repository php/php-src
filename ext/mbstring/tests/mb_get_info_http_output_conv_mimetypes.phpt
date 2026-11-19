--TEST--
mb_get_info() does not release the mbstring.http_output_conv_mimetypes INI value
--EXTENSIONS--
mbstring
--ENV--
MIME_A=text/plain
MIME_B=text/html
FILLER=AAAAAAAAAA
--FILE--
<?php
ini_set('mbstring.http_output_conv_mimetypes', getenv('MIME_A'));
echo "mb_get_info('http_output_conv_mimetypes'): ";
var_dump(mb_get_info('http_output_conv_mimetypes'));
$filler1 = getenv('FILLER');
echo "ini_get(): ";
var_dump(ini_get('mbstring.http_output_conv_mimetypes'));

ini_set('mbstring.http_output_conv_mimetypes', getenv('MIME_B'));
echo "mb_get_info()['http_output_conv_mimetypes']: ";
var_dump(mb_get_info()['http_output_conv_mimetypes']);
$filler2 = getenv('FILLER');
echo "ini_get(): ";
var_dump(ini_get('mbstring.http_output_conv_mimetypes'));
?>
--EXPECT--
mb_get_info('http_output_conv_mimetypes'): string(10) "text/plain"
ini_get(): string(10) "text/plain"
mb_get_info()['http_output_conv_mimetypes']: string(9) "text/html"
ini_get(): string(9) "text/html"
