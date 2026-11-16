--TEST--
rfc1867: a huge number of form variables doesn't reach the hard limit
--SKIPIF--
<?php require __DIR__ . "/oom_rfc1867_skipif.inc"; ?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

oom_rfc1867_request(function ($fh) {
    for ($i = 0; $i < 70000; $i++) {
        oom_rfc1867_part($fh, "Content-Disposition: form-data; name=\"f$i\"", "v$i");
    }
});
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
