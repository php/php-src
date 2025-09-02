--TEST--
Bug GH-11891: fileinfo returns text/xml for some svg files
--EXTENSIONS--
fileinfo
--FILE--
<?php
var_dump(
    (new \finfo(\FILEINFO_MIME_TYPE))->file(
        __DIR__ . '/bug-gh11891.svg',
    ),
);
?>
--EXPECT--
string(13) "image/svg+xml"
