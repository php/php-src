--TEST--
Phar: unsupported compression methods
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup1.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup2.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup3.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup4.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup5.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup6.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup7.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup9.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup10.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup14.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup18.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup19.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup97.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsup98.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/compress_unsupunknown.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
phar error: unsupported compression method (Shrunk) used in this zip in zip-based phar "%scompress_unsup1.zip"
phar error: unsupported compression method (Reduce) used in this zip in zip-based phar "%scompress_unsup2.zip"
phar error: unsupported compression method (Reduce) used in this zip in zip-based phar "%scompress_unsup3.zip"
phar error: unsupported compression method (Reduce) used in this zip in zip-based phar "%scompress_unsup4.zip"
phar error: unsupported compression method (Reduce) used in this zip in zip-based phar "%scompress_unsup5.zip"
phar error: unsupported compression method (Implode) used in this zip in zip-based phar "%scompress_unsup6.zip"
phar error: unsupported compression method (Tokenize) used in this zip in zip-based phar "%scompress_unsup7.zip"
phar error: unsupported compression method (Deflate64) used in this zip in zip-based phar "%scompress_unsup9.zip"
phar error: unsupported compression method (PKWare Implode/old IBM TERSE) used in this zip in zip-based phar "%scompress_unsup10.zip"
phar error: unsupported compression method (LZMA) used in this zip in zip-based phar "%scompress_unsup14.zip"
phar error: unsupported compression method (IBM TERSE) used in this zip in zip-based phar "%scompress_unsup18.zip"
phar error: unsupported compression method (IBM LZ77) used in this zip in zip-based phar "%scompress_unsup19.zip"
phar error: unsupported compression method (WavPack) used in this zip in zip-based phar "%scompress_unsup97.zip"
phar error: unsupported compression method (PPMd) used in this zip in zip-based phar "%scompress_unsup98.zip"
phar error: unsupported compression method (unknown) used in this zip in zip-based phar "%scompress_unsupunknown.zip"
===DONE===
