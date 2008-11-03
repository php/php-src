--TEST--
Bug #44607 (stream_get_line unable to correctly identify the "ending" in the stream content)
--FILE--
<?php
$eol = '<EOL>';
$tempnam = tempnam(sys_get_temp_dir(), 'php');
$data = str_repeat('.', 14000);
$data .= $eol;
$data .= $data;
file_put_contents($tempnam, $data);
$fd = fopen($tempnam, 'r');
var_dump(strlen(stream_get_line($fd, 15000, $eol)));
var_dump(strlen(stream_get_line($fd, 15000, $eol)));
fseek($fd, 1, SEEK_SET);
var_dump(strlen(stream_get_line($fd, 15000, $eol)));
var_dump(strlen(stream_get_line($fd, 15000, $eol)));
unlink($tempnam);
?>
--EXPECT--
int(14000)
int(14000)
int(13999)
int(14000)
