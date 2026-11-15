--TEST--
Stream errors before RINIT do not use the default stream context freed by the previous request
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

$dir = __DIR__ . '/stream_error_default_context';
@mkdir($dir);
file_put_contents("$dir/index.php", '<?php file_get_contents(__FILE__); echo "ok ", count($_POST), "\n";');
/* Larger than SAPI_POST_BLOCK_SIZE, so the request body is spilled to a temporary file */
$body = 'a=' . str_repeat('a', 20000);
/* php-cgi -T reads CONTENT_LENGTH bytes of stdin for each request */
file_put_contents("$dir/body", $body . $body);

$env = [
    'REDIRECT_STATUS' => '1',
    'REQUEST_METHOD' => 'POST',
    'SCRIPT_FILENAME' => "$dir/index.php",
    'CONTENT_TYPE' => 'application/x-www-form-urlencoded',
    'CONTENT_LENGTH' => (string) strlen($body),
];
foreach (['PATH', 'USE_ZEND_ALLOC', 'USE_TRACKED_ALLOC', 'ASAN_OPTIONS', 'LSAN_OPTIONS', 'UBSAN_OPTIONS'] as $name) {
    if (getenv($name) !== false) {
        $env[$name] = getenv($name);
    }
}

/* The first request creates the default stream context, the second one fails to spill its body */
$proc = proc_open(
    [$php, '-n', '-d', 'display_errors=0', '-d', 'log_errors=1', '-d', "sys_temp_dir=$dir/missing", '-T', '2'],
    [0 => ['file', "$dir/body", 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
    $pipes,
    $dir,
    $env
);
$stdout = stream_get_contents($pipes[1]);
$stderr = stream_get_contents($pipes[2]);
proc_close($proc);

var_dump(substr_count($stdout, "ok 0\n"));
echo $stderr;
?>
--CLEAN--
<?php
$dir = __DIR__ . '/stream_error_default_context';
@unlink("$dir/index.php");
@unlink("$dir/body");
@rmdir($dir);
?>
--EXPECTF--
int(2)
PHP Warning:  PHP Request Startup: Unable to create temporary file, Check permissions in temporary files directory. in Unknown on line 0
PHP Warning:  PHP Request Startup: POST data can't be buffered; all data discarded in Unknown on line 0
PHP Warning:  PHP Request Startup: Unable to create temporary file, Check permissions in temporary files directory. in Unknown on line 0
PHP Warning:  PHP Request Startup: POST data can't be buffered; all data discarded in Unknown on line 0

Elapsed time: %s sec
