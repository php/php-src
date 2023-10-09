--TEST--
Bug #74170 locale information change after mime_content_type
--EXTENSIONS--
fileinfo
intl
--SKIPIF--
<?php
if (setlocale(LC_ALL, 'invalid') === 'invalid') { die('skip setlocale() is broken /w musl'); }
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
