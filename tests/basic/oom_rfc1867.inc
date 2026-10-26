<?php

/* Helpers for the oom_rfc1867_*.phpt tests.
 *
 * Each test posts a multipart/form-data body to php-cgi with a small
 * memory_limit and every input limit raised, so that only the memory limit
 * bounds what the parser accumulates. */

const OOM_RFC1867_BOUNDARY = '---------------------------12345';

function oom_rfc1867_part($fh, string $headers, string $body): void
{
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n" . $headers . "\r\n\r\n" . $body . "\r\n");
}

/* Writes $bytes bytes of $char, a chunk at a time, so the test itself doesn't
 * need a multi-megabyte string in memory. */
function oom_rfc1867_fill($fh, int $bytes, string $char = 'A'): void
{
    while ($bytes > 0) {
        $chunk = min($bytes, 1024 * 1024);
        fwrite($fh, str_repeat($char, $chunk));
        $bytes -= $chunk;
    }
}

/* Calls $writeBody() to write the body parts, appends the closing boundary,
 * posts the result and prints what php-cgi reported.
 *
 * $extraIni is passed as additional -d options, for the paths that only run
 * with a given extension configured (mbstring translation, upload progress). */
function oom_rfc1867_request(callable $writeBody, string $memoryLimit = '8M', array $extraIni = [], ?string $target = null): void
{
    $bodyFile = tempnam(sys_get_temp_dir(), 'oom_rfc1867');
    $fh = fopen($bodyFile, 'wb');
    $writeBody($fh);
    fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "--\r\n");
    fclose($fh);

    $cmd = [
        getenv('TEST_PHP_CGI_EXECUTABLE'),
        '-C',
        '-n',
        '-d', 'memory_limit=' . $memoryLimit,
        '-d', 'post_max_size=1G',
        '-d', 'upload_max_filesize=1G',
        '-d', 'max_input_vars=10000000',
        '-d', 'max_file_uploads=1000000',
        '-d', 'max_multipart_body_parts=10000000',
        '-d', 'max_input_time=-1',
        '-d', 'html_errors=0',
        '-d', 'fatal_error_backtraces=Off',
    ];

    foreach ($extraIni as $ini) {
        $cmd[] = '-d';
        $cmd[] = $ini;
    }

    $env = array_merge($_ENV, [
        'REDIRECT_STATUS' => '1',
        'CONTENT_TYPE' => 'multipart/form-data; boundary=' . OOM_RFC1867_BOUNDARY,
        'CONTENT_LENGTH' => filesize($bodyFile),
        'REQUEST_METHOD' => 'POST',
        'SCRIPT_FILENAME' => $target ?? __DIR__ . '/oom_rfc1867_target.inc',
    ]);

    $spec = [
        0 => ['file', $bodyFile, 'r'],
        1 => ['pipe', 'w'],
        2 => ['pipe', 'w'],
    ];

    $pipes = [];
    $handle = proc_open($cmd, $spec, $pipes, getcwd(), $env);
    $out = stream_get_contents($pipes[1]);
    $err = stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($handle);
    unlink($bodyFile);

    /* Drop the CGI headers, keep the body. */
    $out = preg_replace("/^.*?\r\n\r\n/s", '', $out);
    echo trim(str_replace("\r\n", "\n", $out)), "\n";
    if ($err !== '') {
        echo 'stderr: ', trim($err), "\n";
    }
}
