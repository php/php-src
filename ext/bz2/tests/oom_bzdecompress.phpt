--TEST--
bzdecompress() doesn't reach hard limit
--EXTENSIONS--
bz2
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die("skip ASan-instrumented realloc() is slow");
?>
--FILE--
<?php
// Expands to 256 MB
bzdecompress(base64_decode('QlpoOTFBWSZTWQ4J4t8BX45AAMAAAAggADCATUZCoCWpCoCXMUFZJlNZDgni3wFfjkAAwAAACCAAMIBNRkKgJakKgJcxQVkmU1kOCeLfAV+OQADAAAAIIAAwgE1GQqAlqQqAlzFBWSZTWQ4J4t8BX45AAMAAAAggADCATUZCoCWpCoCXMUFZJlNZDgni3wFfjkAAwAAACCAAMIBNRkKgJakKgJcxQVkmU1kezuTbASo/wADABAAIIAAwzAUppqqoSRsAIkjxdyRThQkGsXyvAA=='));
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
