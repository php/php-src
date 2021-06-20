--TEST--
zlib.output_compression=1 with client not accepting compression
--EXTENSIONS--
zlib
--INI--
zlib.output_compression=1
display_startup_errors=1
--FILE--
===DONE===
--EXPECT--
===DONE===
