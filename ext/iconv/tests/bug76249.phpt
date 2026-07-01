--TEST--
Bug #76249 (stream filter convert.iconv leads to infinite loop on invalid sequence)
--EXTENSIONS--
iconv
--FILE--
<?php
$ignore = "";
if (ICONV_IMPL == "libiconv" || ICONV_IMPL == "glibc") {
    // The original bug report used "//IGNORE", and the bug itself
    // involves the return value and errno from iconv(), so in the
    // interest of fidelity we include the suffix on systems like the
    // one where the bug was reported. On other systems however,
    // despite being mentioned in POSIX 2024, the "//IGNORE" suffix is
    // not supported consistently. Musl in particular does not support
    // it at all, so we must omit it to avoid triggering other
    // (unexpected) errors. In any case, it is nice to check that
    // iconv and invalid streams do not interact badly.
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
