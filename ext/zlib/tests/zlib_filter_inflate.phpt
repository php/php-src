--TEST--
zlib.inflate (with convert.base64-decode)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php
$text = 'HctBDoAgDETRq8zOjfEeHKOGATG0TRpC4u1Vdn/xX4IoxkVMxgP1zA4vkJVhULk9UGkM6TvSNolmxUNlNLePVQ45O3eINf0fsQxtCxwv';

$fp = fopen('php://stdout', 'w');
stream_filter_append($fp, 'convert.base64-decode', STREAM_FILTER_WRITE);
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_WRITE);
fwrite($fp, $text);
fclose($fp);

?>
--EXPECT--
I am the very model of a modern major general, I've information vegetable, animal, and mineral.
