--TEST--
GH-17399 (iconv memory leak with large line-length in iconv_mime_encode)
--EXTENSIONS--
iconv
--FILE--
<?php
$options = [
    'line-length' => PHP_INT_MAX,
];
iconv_mime_encode('Subject', 'test', $options);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s in %s on line %d
