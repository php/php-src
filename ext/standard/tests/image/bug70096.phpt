--TEST--
Bug #70096 (Repeated iptcembed() adds superfluous FF bytes)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!function_exists('imagejpeg')) die('skip imagejpeg not available');
?>
--FILE--
<?php
$filename = __DIR__ . '/bug70096.jpg';
$im = imagecreatetruecolor(10, 10);
imagejpeg($im, $filename);
imagedestroy($im);
$data = "\x1C\x02x\x00\x0ATest image"
    . "\x1C\x02t\x00\x22Copyright 2008-2009, The PHP Group";
$content1 = iptcembed($data, $filename);
$fp = fopen($filename, "wb");
fwrite($fp, $content1);
fclose($fp);
$content2 = iptcembed($data, $filename);
$fp = fopen($filename, "wb");
fwrite($fp, $content2);
fclose($fp);
var_dump($content1 === $content2);
?>
--CLEAN--
<?php
$filename = __DIR__ . '/bug70096.jpg';
@unlink($filename);
?>
--EXPECT--
bool(true)
