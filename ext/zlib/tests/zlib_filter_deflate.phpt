--TEST--
zlib.deflate (with convert.base64-encode)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php /* $Id$ */
$text = 'I am the very model of a modern major general, I\'ve information vegetable, animal, and mineral.';

$fp = fopen('php://stdout', 'w');
stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE);
stream_filter_append($fp, 'convert.base64-encode', STREAM_FILTER_WRITE);
fwrite($fp, $text);
fclose($fp);

?> 
--EXPECT-- 
eJwdy0EOgCAMRNGrzM6N8R4co4YBMbRNGkLi7VV2f/FfgijGRUzGA/XMDi+QlWFQuT1QaQzpO9I2iWbFQ2U0t49VDjk7d4g1/R+xDG0LHC8+diGh
