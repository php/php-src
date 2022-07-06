--TEST--
Bug #74170 locale information change after mime_content_type
--SKIPIF--
<?php
if (!class_exists('finfo'))
    die('skip no fileinfo extension');
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
if (setlocale(LC_CTYPE, 'ru_RU.koi8r') === false)
    die('skip ru_RU.koi8r locale is not available');
?>
--FILE--
<?php
var_dump(setlocale(LC_CTYPE, 'ru_RU.koi8r'));
var_dump(nl_langinfo(CODESET));
var_dump(mime_content_type(__DIR__ . '/resources/test.ppt'));
var_dump(nl_langinfo(CODESET));
?>
--EXPECT--
string(11) "ru_RU.koi8r"
string(6) "KOI8-R"
string(29) "application/vnd.ms-powerpoint"
string(6) "KOI8-R"
