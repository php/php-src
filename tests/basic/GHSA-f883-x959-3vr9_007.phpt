--TEST--
GHSA-f883-x959-3vr9 007
--SKIPIF--
<?php
if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
    die("skip php-cgi not available");
}
?>
--FILE--
<?php

$cmd = [
    getenv('TEST_PHP_CGI_EXECUTABLE'),
    '-C',
    '-n',
    __DIR__ . '/GHSA-f883-x959-3vr9_007.inc',
];

$body = ""
    . "--b\r\n"
    . "Content-Disposition: form-data; name=\"d1\"\r\n"
    . str_repeat("A:b\r\n", 600*1000)
    . "\r\n"
    . "test\r\n"
    . "--b--\r\n"
    ;

$env = array_merge($_ENV, [
    'REDIRECT_STATUS' => '1',
    'CONTENT_TYPE' => "multipart/form-data; boundary=b",
    'CONTENT_LENGTH' => strlen($body),
    'REQUEST_METHOD' => 'POST',
    'SCRIPT_FILENAME' => __DIR__ . '/GHSA-f883-x959-3vr9_007.inc',
]);

$spec = [
    0 => ['pipe', 'r'],
    1 => STDOUT,
    2 => STDOUT,
];

$pipes = [];

print "Starting...\n";

$handle = proc_open($cmd, $spec, $pipes, getcwd(), $env);

fwrite($pipes[0], $body);

$status = proc_close($handle);

print "\n";

?>
--EXPECTF--
Starting...
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

array(1) {
  ["d1"]=>
  string(4) "test"
}
