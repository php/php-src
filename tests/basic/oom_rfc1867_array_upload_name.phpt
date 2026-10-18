--TEST--
rfc1867: a huge array upload field name doesn't reach the hard limit
--SKIPIF--
<?php require __DIR__ . "/oom_rfc1867_skipif.inc"; ?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

oom_rfc1867_request(function ($fh) {
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
        . 'Content-Disposition: form-data; name="');
    for ($i = 0; $i < 5; $i++) {
        fwrite($fh, str_repeat('A', 1024 * 1024));
    }
    fwrite($fh, "[k]\"; filename=\"f.txt\"\r\nContent-Type: text/plain\r\n\r\nx\r\n");
}, '16M');

// Same, with the field name in the array index instead.
oom_rfc1867_request(function ($fh) {
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
        . 'Content-Disposition: form-data; name="u[');
    for ($i = 0; $i < 5; $i++) {
        fwrite($fh, str_repeat('A', 1024 * 1024));
    }
    fwrite($fh, "]\"; filename=\"f.txt\"\r\nContent-Type: text/plain\r\n\r\nx\r\n");
}, '16M');

// Base name and array index both huge: abuf and array_index are each a copy
// of param, retained together with param and the header value.
foreach ([1, 2] as $mb) {
    oom_rfc1867_request(function ($fh) use ($mb) {
        fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
            . 'Content-Disposition: form-data; name="');
        oom_rfc1867_fill($fh, $mb * 1024 * 1024);
        fwrite($fh, '[');
        oom_rfc1867_fill($fh, $mb * 1024 * 1024);
        fwrite($fh, "]\"; filename=\"f.txt\"\r\nContent-Type: text/plain\r\n\r\nx\r\n");
    }, '8M');
}
?>
--EXPECTF--
Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
