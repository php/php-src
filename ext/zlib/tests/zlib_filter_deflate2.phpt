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
ËA€ DÑ«ÌÎñ£†1´MBâíUvñ_‚(ÆELÆõÌ/•aP¹=Pié;Ò6‰fÅCe4·U9;wˆ5ı±m/ 
