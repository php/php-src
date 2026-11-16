--TEST--
rfc1867: a part with a huge number of headers doesn't reach the hard limit
--SKIPIF--
<?php require __DIR__ . "/oom_rfc1867_skipif.inc"; ?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

oom_rfc1867_request(function ($fh) {
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n");
    for ($i = 0; $i < 150000; $i++) {
        fwrite($fh, "X-$i: v\r\n");
    }
    fwrite($fh, "Content-Disposition: form-data; name=\"f\"\r\n\r\nv\r\n");
});
?>
--EXPECT--
post=1 files=0
