--TEST--
zlib.inflate (with convert.base64-decode)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php /* $Id$ */
$text = 'eJwdy0EOgCAMRNGrzM6N8R4co4YBMbRNGkLi7VV2f/FfgijGRUzGA/XMDi+QlWFQuT1QaQzpO9I2iWbFQ2U0t49VDjk7d4g1/R+xDG0LHC8+diGh';

$fp = fopen('php://stdout', 'w');
stream_filter_append($fp, 'convert.base64-decode', STREAM_FILTER_WRITE);
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_WRITE);
fwrite($fp, $text);
fclose($fp);

?> 
--EXPECT-- 
I am the very model of a modern major general, I've information vegetable, animal, and mineral.
