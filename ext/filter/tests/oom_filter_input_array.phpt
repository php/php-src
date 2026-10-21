--TEST--
filter_input_array() doesn't reach hard limit
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
filter_input_array(INPUT_POST, ['k' => ['filter' => FILTER_SANITIZE_SPECIAL_CHARS, 'flags' => FILTER_FLAG_ENCODE_HIGH]]);
echo "no OOM\n";
TARGET);

$boundary = '----X';
$body = "--$boundary\r\nContent-Disposition: form-data; name=\"k\"\r\n\r\n"
      . str_repeat("\xFF", 2 * 1024 * 1024)
      . "\r\n--$boundary--\r\n";

$p = proc_open(
    [getenv('TEST_PHP_CGI_EXECUTABLE'), '-n', '-d', 'memory_limit=12M', '-d', 'html_errors=0', '-d', 'fatal_error_backtraces=Off'],
    [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
    $pipes,
    null,
    [
        'REQUEST_METHOD'  => 'POST',
        'CONTENT_TYPE'    => "multipart/form-data; boundary=$boundary",
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
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.target.php');
?>
--EXPECTF--
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8


Fatal error: Allowed memory size of 12582912 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
