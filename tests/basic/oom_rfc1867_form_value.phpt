--TEST--
rfc1867: a huge form variable value doesn't reach the hard limit
--SKIPIF--
<?php require __DIR__ . "/oom_rfc1867_skipif.inc"; ?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

oom_rfc1867_request(function ($fh) {
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
        . "Content-Disposition: form-data; name=\"f\"\r\n\r\n");
    for ($i = 0; $i < 6; $i++) {
        fwrite($fh, str_repeat('x', 1024 * 1024));
    }
    fwrite($fh, "\r\n");
});
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
