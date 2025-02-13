--TEST--
Memory leak when passing a dictionary with invalid encoding
--EXTENSIONS--
zlib
--FILE--
<?php
try {
	inflate_init(123456, ["dictionary" => "dict"]);
} catch (ValueError $e) {
	echo $e->getMessage(), "\n";
}
try {
	deflate_init(123456, ["dictionary" => "dict"]);
} catch (ValueError $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Encoding mode must be ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE
deflate_init(): Argument #1 ($encoding) must be one of ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP, or ZLIB_ENCODING_DEFLATE
