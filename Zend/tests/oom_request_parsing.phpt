--TEST--
Request parsing doesn't reach hard limit
--SKIPIF--
<?php
if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
    die("skip php-cgi not available");
}
if (getenv('SKIP_ASAN')) {
    die("skip ASan-instrumented realloc() is slow");
}
?>
--FILE--
<?php
$base = __DIR__ . '/' . basename(__FILE__, '.php');

$tiny = $base . '.target.php';
file_put_contents($tiny, <<<'TARGET'
<?php
echo "no OOM\n";
TARGET);

$big = $base . '.big.target.php';
file_put_contents($big, "<?php\n" . str_repeat("\$x = 1 + 2;\n", 60000) . "echo \"no OOM\\n\";\n");

/* The body goes through a file rather than a pipe: some of these are several
 * megabytes, and the child stops reading as soon as it reports the error. */
function oom_post(string $body, string $script, array $ini): void
{
    $bodyFile = tempnam(sys_get_temp_dir(), 'oom_request_parsing');
    file_put_contents($bodyFile, $body);

    $cmd = [getenv('TEST_PHP_CGI_EXECUTABLE'), '-n', '-d', 'fatal_error_backtraces=Off'];
    foreach ($ini as $entry) {
        $cmd[] = '-d';
        $cmd[] = $entry;
    }

    $p = proc_open(
        $cmd,
        [['file', $bodyFile, 'r'], STDOUT, STDERR],
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
    proc_close($p);
    unlink($bodyFile);
}

/* Deeply nested variable names, at the default max_input_vars and
 * max_input_nesting_level. */
oom_post(
    implode('&', array_fill(0, 1000, 'a[]' . str_repeat('[b]', 63) . '=c')),
    $tiny,
    ['memory_limit=8M', 'html_errors=0']
);

$ini = ['memory_limit=8M', 'post_max_size=1G', 'max_input_vars=10000000', 'html_errors=0'];

/* One value with no '&' in it: nothing consumes it, so add_post_vars() never
 * gets a complete variable to check and only the read loop can notice. */
$oneValue = 'a=' . str_repeat('x', 6 * 1024 * 1024);

/* Many small variables: the limit is reached while registering them. */
$manyVars = '';
for ($i = 0; $i < 200000; $i++) {
    $manyVars .= ($i ? '&' : '') . "k$i=v$i";
}

/* A big value followed by many small ones: add_post_var() copies the value
 * once with estrndup() and again when registering it, and each half fits even
 * though the pair doesn't. */
$mixed = 'a=' . str_repeat('x', 3 * 1024 * 1024);
for ($i = 0; $i < 100000; $i++) {
    $mixed .= "&k$i=v$i";
}

foreach ([$oneValue, $manyVars, $mixed] as $body) {
    foreach ([$tiny, $big] as $script) {
        oom_post($body, $script, $ini);
    }
}
?>
--CLEAN--
<?php
$base = __DIR__ . '/' . basename(__FILE__, '.clean.php');
@unlink($base . '.target.php');
@unlink($base . '.big.target.php');
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
