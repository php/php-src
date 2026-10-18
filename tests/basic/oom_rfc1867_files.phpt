--TEST--
rfc1867: a huge number of file parts doesn't reach the hard limit
--SKIPIF--
<?php require __DIR__ . "/oom_rfc1867_skipif.inc"; ?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

// Many tiny uploads, with a memory_limit of 8 MB (table and $_FILES)
oom_rfc1867_request(function ($fh) {
    for ($i = 0; $i < 30000; $i++) {
        oom_rfc1867_part($fh,
            "Content-Disposition: form-data; name=\"u$i\"; filename=\"f$i.txt\"\r\nContent-Type: text/plain",
            'x');
    }
});

// Same, as an array upload (lbuf/abuf)
oom_rfc1867_request(function ($fh) {
    for ($i = 0; $i < 30000; $i++) {
        oom_rfc1867_part($fh,
            "Content-Disposition: form-data; name=\"u[k$i]\"; filename=\"f$i.txt\"\r\nContent-Type: text/plain",
            'x');
    }
});
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
