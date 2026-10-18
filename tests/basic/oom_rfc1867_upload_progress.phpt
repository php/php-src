--TEST--
rfc1867: the session upload progress handler doesn't reach the hard limit
--SKIPIF--
<?php
require __DIR__ . "/oom_rfc1867_session_skipif.inc";
if (getenv('SKIP_ASAN')) die("skip slow under ASan");
?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

$savePath = sys_get_temp_dir() . '/oom_rfc1867_sess';
@mkdir($savePath);

$sessionIni = [
    'session.upload_progress.enabled=1',
    'session.save_path=' . $savePath,
    /* accept the session id we make up below */
    'session.use_strict_mode=0',
    /* update on every chunk, so FILE_DATA re-encodes as often as possible */
    'session.upload_progress.freq=1',
    'session.upload_progress.min_freq=0',
];

/* The progress handler only engages once it has seen both the session id and
 * the progress key, so those two parts come first. */
function oom_rfc1867_progress_preamble($fh): void
{
    oom_rfc1867_part($fh, 'Content-Disposition: form-data; name="PHPSESSID"',
        'abcdef0123456789abcdef0123456789');
    oom_rfc1867_part($fh, 'Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"', 'k');
}

/* Many small file parts: $_SESSION[...]['files'] grows per part, and the whole
 * session is re-encoded each time. */
foreach ([4000, 8000] as $parts) {
    oom_rfc1867_request(function ($fh) use ($parts) {
        oom_rfc1867_progress_preamble($fh);
        for ($i = 0; $i < $parts; $i++) {
            oom_rfc1867_part($fh,
                "Content-Disposition: form-data; name=\"u$i\"; filename=\"f$i.txt\"\r\nContent-Type: text/plain",
                'xxxxxxxx');
        }
    }, '4M', $sessionIni);
}

/* Fewer parts, but each one makes the encoded session much bigger. */
foreach ([[1000, 1024], [2000, 512]] as [$parts, $nameLen]) {
    oom_rfc1867_request(function ($fh) use ($parts, $nameLen) {
        oom_rfc1867_progress_preamble($fh);
        for ($i = 0; $i < $parts; $i++) {
            $name = str_pad("u$i", $nameLen, 'A');
            oom_rfc1867_part($fh,
                "Content-Disposition: form-data; name=\"$name\"; filename=\"$name.txt\"\r\nContent-Type: text/plain",
                'xxxxxxxx');
        }
    }, '8M', $sessionIni);
}

/* A single part whose field name and file name are both huge: the handler
 * copies both into the progress array before the parser's own checks run. */
oom_rfc1867_request(function ($fh) {
    oom_rfc1867_progress_preamble($fh);
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
        . 'Content-Disposition: form-data; name="');
    oom_rfc1867_fill($fh, 1024 * 1024);
    fwrite($fh, '"; filename="');
    oom_rfc1867_fill($fh, 1024 * 1024);
    fwrite($fh, "\"\r\nContent-Type: text/plain\r\n\r\nxxxx\r\n");
}, '8M', $sessionIni);

/* A huge PHP_SESSION_UPLOAD_PROGRESS value: the handler copies it into the
 * progress key on MULTIPART_EVENT_FORMDATA, so the soft limit is reached
 * inside the handler rather than in the parser. */
oom_rfc1867_request(function ($fh) {
    oom_rfc1867_part($fh, 'Content-Disposition: form-data; name="PHPSESSID"',
        'abcdef0123456789abcdef0123456789');
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
        . "Content-Disposition: form-data; name=\"PHP_SESSION_UPLOAD_PROGRESS\"\r\n\r\n");
    oom_rfc1867_fill($fh, 1024 * 1024);
    fwrite($fh, "\r\n");
    oom_rfc1867_part($fh,
        "Content-Disposition: form-data; name=\"u\"; filename=\"f.txt\"\r\nContent-Type: text/plain",
        'x');
}, '4M', $sessionIni);

foreach (glob($savePath . '/*') as $f) {
    @unlink($f);
}
@rmdir($savePath);
?>
--EXPECTF--
Fatal error: Allowed memory size of 4194304 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 4194304 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 4194304 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
