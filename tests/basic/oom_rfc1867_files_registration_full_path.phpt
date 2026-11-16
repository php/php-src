--TEST--
rfc1867: reaching the soft limit on the $_FILES [full_path] entry
--SKIPIF--
<?php
require __DIR__ . "/oom_rfc1867_skipif.inc";
if (getenv('SKIP_ASAN')) die("skip difficult to produce on ASAN");
?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

/* Huge file name: the weight lands on the [full_path] entry. */
oom_rfc1867_request(function ($fh) {
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
        . "Content-Disposition: form-data; name=\"pad\"\r\n\r\n");
    oom_rfc1867_fill($fh, 1344 * 1024);
    fwrite($fh, "\r\n");

    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
        . 'Content-Disposition: form-data; name="u"; filename="');
    oom_rfc1867_fill($fh, 1200 * 1024);
    fwrite($fh, "\"\r\nContent-Type: text/plain\r\n\r\nx\r\n");
});
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
