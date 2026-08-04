--TEST--
Bug #76249 (stream filter convert.iconv leads to infinite loop on invalid sequence)
--EXTENSIONS--
iconv
--FILE--
<?php
$ignore = "";
if (ICONV_IMPL == "libiconv" || ICONV_IMPL == "glibc") {
    // The original bug report uses "//IGNORE", and the bug itself
    // involves the return value and errno from iconv(), so in the
    // interest of fidelity we include the suffix on systems like the
    // one where the bug was reported. On other systems however, the
    // "//IGNORE" suffix may not be supported, and this is allowed by
    // POSIX (musl in particular does not support it).
    $ignore = "//IGNORE";
}

$fh = fopen('php://memory', 'rw');
fwrite($fh, "abc");
rewind($fh);
if (false === @stream_filter_append($fh, "convert.iconv.ucs-2/utf8{$ignore}", STREAM_FILTER_READ, [])) {
    stream_filter_append($fh, "convert.iconv.ucs-2/utf-8{$ignore}", STREAM_FILTER_READ, []);
}
var_dump(stream_get_contents($fh));
?>
DONE
--EXPECTREGEX--
Warning: stream_get_contents\(\): iconv stream filter \("ucs-2"=>"utf-?8(\/\/IGNORE)?"\): invalid multibyte sequence in .*bug76249\.php on line \d+
string\(0\) ""
DONE
