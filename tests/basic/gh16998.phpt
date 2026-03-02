--TEST--
GH-16998 (UBSAN warning in rfc1867)
--SKIPIF--
<?php
if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
    die("skip php-cgi not available");
}
?>
--FILE--
<?php
const FILLUNIT = 5 * 1024;
$cmd = [
    getenv('TEST_PHP_CGI_EXECUTABLE'),
    '-C',
    '-n',
    __DIR__ . '/GHSA-9pqp-7h25-4f32.inc',
];
$boundary = str_repeat('A', FILLUNIT);
$body = ""
    . "--$boundary\r\n"
    . "Content-Disposition: form-data; name=\"koko\"\r\n"
    . "\r\n"
    . "BBB\r\n--" . substr($boundary, 0, -1) . "CCC\r\n"
    . "--$boundary--\r\n"
    ;
$env = array_merge($_ENV, [
    'REDIRECT_STATUS' => '1',
    'CONTENT_TYPE' => "multipart/form-data; boundary=",
    'CONTENT_LENGTH' => strlen($body),
    'REQUEST_METHOD' => 'POST',
    'SCRIPT_FILENAME' => __DIR__ . '/GHSA-9pqp-7h25-4f32.inc',
]);
$spec = [
    0 => ['pipe', 'r'],
    1 => STDOUT,
    2 => STDOUT,
];
$pipes = [];
$handle = proc_open($cmd, $spec, $pipes, getcwd(), $env);
fwrite($pipes[0], $body);
proc_close($handle);
?>
--EXPECTF--
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

Hello world
array(0) {
}
