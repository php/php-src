--TEST--
rfc1867: a huge header value built from continuation lines doesn't reach the hard limit
--SKIPIF--
<?php require __DIR__ . "/oom_rfc1867_skipif.inc"; ?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

oom_rfc1867_request(function ($fh) {
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\nContent-Type: pad\r\n");
    for ($i = 0; $i < 2500; $i++) {
        fwrite($fh, ' ' . str_repeat('a', 4000) . "\r\n");
    }
    fwrite($fh, "Content-Disposition: form-data; name=\"f\"\r\n\r\nv\r\n");
});
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
