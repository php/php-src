--TEST--
zlib.deflate (with level parameter set)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php
$text = 'I am the very model of a modern major general, I\'ve information vegetable, animal, and mineral.';

$fp = fopen('php://stdout', 'w');
stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, array('level' => 9));
fwrite($fp, $text);
fclose($fp);

?>
--EXPECT--
�A� Dѫ�΍���1�MB��Uv�_�(�EL���/��aP�=Pi�;�6�f�Ce4��U9;w�5��m/ 
