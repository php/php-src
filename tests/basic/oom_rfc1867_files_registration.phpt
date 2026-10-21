--TEST--
rfc1867: reaching the soft limit at each step of the $_FILES registration
--SKIPIF--
<?php
require __DIR__ . "/oom_rfc1867_skipif.inc";
if (getenv('SKIP_ASAN')) die("skip difficult to produce on ASAN");
?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

function oom_rfc1867_pad($fh, int $bytes): void
{
    if ($bytes <= 0) {
        return;
    }
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
        . "Content-Disposition: form-data; name=\"pad\"\r\n\r\n");
    oom_rfc1867_fill($fh, $bytes);
    fwrite($fh, "\r\n");
}

/* How much padding it takes to leave a given amount of room depends on how the
 * allocator accounts memory: Zend MM counts whole chunks, while the tracked
 * allocator used under ASan counts exact sizes. */
$padsKb = getenv('USE_ZEND_ALLOC') === '0'
    ? [1400, 1700, 2000, 2300]
    : [200, 292, 636, 1340];

/* name="<700K>[k]" - the field name carries the weight. */
foreach ($padsKb as $padKb) {
    oom_rfc1867_request(function ($fh) use ($padKb) {
        oom_rfc1867_pad($fh, $padKb * 1024);
        fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
            . 'Content-Disposition: form-data; name="');
        oom_rfc1867_fill($fh, 700 * 1024);
        fwrite($fh, "[k]\"; filename=\"f.txt\"\r\nContent-Type: text/plain\r\n\r\nx\r\n");
    });
}

/* name="u[<700K>]" - the weight is in the array index instead. */
oom_rfc1867_request(function ($fh) {
    oom_rfc1867_pad($fh, 1344 * 1024);
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
        . 'Content-Disposition: form-data; name="u[');
    oom_rfc1867_fill($fh, 700 * 1024);
    fwrite($fh, "]\"; filename=\"f.txt\"\r\nContent-Type: text/plain\r\n\r\nx\r\n");
});

/* The [full_path] entry is in oom_rfc1867_files_registration_full_path.phpt:
 * it needs a padding that only Zend MM's accounting can produce. */
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
