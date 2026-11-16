--TEST--
rfc1867: a huge Content-Disposition line doesn't reach the hard limit
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
    fwrite($fh, "\"\r\n\r\nv\r\n");
});

oom_rfc1867_request(function ($fh) {
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
        . 'Content-Disposition: form-data; name="u"; filename="');
    for ($i = 0; $i < 6; $i++) {
        fwrite($fh, str_repeat('A', 1024 * 1024));
    }
    fwrite($fh, ".txt\"\r\nContent-Type: text/plain\r\n\r\nx\r\n");
});

/* Both the field name and the file name huge in the same Content-Disposition:
 * getword_conf() keeps a copy of each, so param and filename are retained at
 * the same time on top of the header value they were parsed out of. */
foreach ([2, 3] as $mb) {
    oom_rfc1867_request(function ($fh) use ($mb) {
        fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
            . 'Content-Disposition: form-data; name="');
        oom_rfc1867_fill($fh, $mb * 1024 * 1024);
        fwrite($fh, '"; filename="');
        oom_rfc1867_fill($fh, $mb * 1024 * 1024);
        fwrite($fh, "\"\r\nContent-Type: text/plain\r\n\r\nx\r\n");
    });
}
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
