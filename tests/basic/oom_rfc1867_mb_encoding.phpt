--TEST--
rfc1867: mbstring input conversion of a body part doesn't reach the hard limit
--SKIPIF--
<?php require __DIR__ . "/oom_rfc1867_mb_skipif.inc"; ?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

const MB_INI = [
    'mbstring.encoding_translation=1',
    'internal_encoding=UTF-8',
    'mbstring.http_input=CP1252',
    /* The conversion is what is under test, not the deprecation notice. */
    'error_reporting=' . (E_ALL & ~E_DEPRECATED),
];

/* Sweep the size: the interesting case is the one where the part still fits
 * under the soft limit and the conversion is what pushes past it, so the
 * window matters. */
for ($kb = 1216; $kb <= 1664; $kb += 32) {
    oom_rfc1867_request(function ($fh) use ($kb) {
        fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
            . "Content-Disposition: form-data; name=\"f\"\r\n\r\n");
        oom_rfc1867_fill($fh, $kb * 1024, "\x80");
        fwrite($fh, "\r\n");
    }, '8M', MB_INI);
}

/* Same conversion, on the field name (getword_conf result) ... */
foreach ([1664, 1792] as $kb) {
    oom_rfc1867_request(function ($fh) use ($kb) {
        fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
            . 'Content-Disposition: form-data; name="');
        oom_rfc1867_fill($fh, $kb * 1024, "\x80");
        fwrite($fh, "\"\r\n\r\nv\r\n");
    }, '8M', MB_INI);
}

/* ... and on the file name. */
foreach ([1664, 1792] as $kb) {
    oom_rfc1867_request(function ($fh) use ($kb) {
        fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
            . 'Content-Disposition: form-data; name="u"; filename="');
        oom_rfc1867_fill($fh, $kb * 1024, "\x80");
        fwrite($fh, "\"\r\nContent-Type: text/plain\r\n\r\nx\r\n");
    }, '8M', MB_INI);
}

/* Bigger expansion factors, through a different mbfl encoder. */
foreach ([704, 736, 960, 1248, 1472, 1632] as $kb) {
    oom_rfc1867_request(function ($fh) use ($kb) {
        fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
            . "Content-Disposition: form-data; name=\"f\"\r\n\r\n");
        /* ISO-8859-8 0xE0 -> "&#1488;": one byte in, seven bytes out. */
        oom_rfc1867_fill($fh, $kb * 1024, "\xE0");
        fwrite($fh, "\r\n");
    }, '8M', [
        'mbstring.encoding_translation=1',
        'internal_encoding=HTML-ENTITIES',
        'mbstring.http_input=ISO-8859-8',
        'error_reporting=' . (E_ALL & ~E_DEPRECATED),
    ]);
}

/* UTF-32: a flat 4x on every input byte. The window where the conversion is
 * what reaches the limit - rather than the body read before it - depends on how
 * the allocator accounts memory, and Zend MM's chunks and the tracked
 * allocator's exact sizes only overlap in a single point. */
$utf32Kb = getenv('USE_ZEND_ALLOC') === '0'
    ? [1152, 1280]
    : [896, 1280];

foreach ($utf32Kb as $kb) {
    oom_rfc1867_request(function ($fh) use ($kb) {
        fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
            . "Content-Disposition: form-data; name=\"f\"\r\n\r\n");
        oom_rfc1867_fill($fh, $kb * 1024, "\x80");
        fwrite($fh, "\r\n");
    }, '8M', [
        'mbstring.encoding_translation=1',
        'internal_encoding=UTF-32',
        'mbstring.http_input=CP1252',
        'error_reporting=' . (E_ALL & ~E_DEPRECATED),
    ]);
}
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
