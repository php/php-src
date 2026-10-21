--TEST--
rfc1867: a huge form field name doesn't reach the hard limit
--SKIPIF--
<?php require __DIR__ . "/oom_rfc1867_skipif.inc"; ?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

for ($kb = 5 * 1024; $kb <= 6 * 1024 + 512; $kb += 256) {
    oom_rfc1867_request(function ($fh) use ($kb) {
        fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
            . 'Content-Disposition: form-data; name="'
            . str_repeat('A', $kb * 1024)
            . "\"\r\n\r\nv\r\n");
    }, '10M');
}
?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
