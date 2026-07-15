--TEST--
compress.zlib:// wrapper with invalid compression level
--EXTENSIONS--
zlib
--SKIPIF--
<?php
in_array('compress.zlib', stream_get_wrappers()) || die('skip No zlib wrapper');
?>
--FILE--
<?php declare(strict_types=1);

$filename = tempnam(sys_get_temp_dir(), "php-zlib-test-wrapper-level-error");
$thisfile = file_get_contents(__FILE__);

function write_at_level(mixed $level) {
	global $filename, $thisfile;

	$ctx = stream_context_create();
	stream_context_set_option($ctx, 'zlib', 'level', $level);
	$fp = fopen("compress.zlib://$filename", 'w', false, $ctx);
	for ($i = 0; $i < 10; ++$i) {
		fwrite($fp, $thisfile);
	}
	fclose($fp);
	$size = filesize($filename);
	unlink($filename);
	return $size;
}

$size_string_type = write_at_level("not a number");
var_dump($size_string_type);

$size_array_type = write_at_level(new stdClass());
var_dump($size_array_type);

$size_oob = write_at_level(10000);
var_dump($size_oob);

?>
--EXPECTF--
int(%d)

Warning: Object of class stdClass could not be converted to int in %s on line %d
int(%d)
int(0)
