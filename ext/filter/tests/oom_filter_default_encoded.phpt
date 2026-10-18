--TEST--
filter.default=encoded doesn't reach hard limit while parsing POST data
--EXTENSIONS--
filter
--SKIPIF--
<?php
if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
    die("skip php-cgi not available");
}
?>
--FILE--
<?php
$script = __DIR__ . '/' . basename(__FILE__, '.php') . '.target.php';
file_put_contents($script, <<<'TARGET'
<?php
echo "no OOM\n";
TARGET);

function oom_post_encoded(string $body, string $script): void
{
    $p = proc_open(
        [
            getenv('TEST_PHP_CGI_EXECUTABLE'), '-n',
            '-d', 'memory_limit=8M',
            '-d', 'post_max_size=1G',
            '-d', 'html_errors=0',
            '-d', 'filter.default=encoded',
            '-d', 'filter.default_flags=0',
            '-d', 'fatal_error_backtraces=Off',
            /* the conversion is what is under test, not the deprecation */
            '-d', 'error_reporting=' . (E_ALL & ~E_DEPRECATED),
        ],
        [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
        $pipes,
        null,
        [
            'REQUEST_METHOD'  => 'POST',
            'CONTENT_TYPE'    => 'application/x-www-form-urlencoded',
            'CONTENT_LENGTH'  => (string) strlen($body),
            'SCRIPT_FILENAME' => $script,
            'REDIRECT_STATUS' => '1',
        ]
    );
    fwrite($pipes[0], $body);
    fclose($pipes[0]);
    $out = stream_get_contents($pipes[1]) . stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($p);

    echo $out;
}

for ($kb = 2016; $kb <= 2064; $kb += 4) {
    oom_post_encoded('a=' . str_repeat('x', $kb * 1024), $script);
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.target.php');
?>
--EXPECTF--
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
