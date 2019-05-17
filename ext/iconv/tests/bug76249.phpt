--TEST--
Bug #76249 (stream filter convert.iconv leads to infinite loop on invalid sequence)
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--FILE--
<?php
$fh = fopen('php://memory', 'rw');
fwrite($fh, "abc");
rewind($fh);
if (false === @stream_filter_append($fh, 'convert.iconv.ucs-2/utf8//IGNORE', STREAM_FILTER_READ, [])) {
	stream_filter_append($fh, 'convert.iconv.ucs-2/utf-8//IGNORE', STREAM_FILTER_READ, []);
}
$a = stream_get_contents($fh);
var_dump(strlen($a));
?>
DONE
--EXPECTF--
Warning: stream_get_contents(): iconv stream filter ("ucs-2"=>"utf%A8//IGNORE"): invalid multibyte sequence in %sbug76249.php on line %d
int(3)
DONE
