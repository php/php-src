--TEST--
POST Content-Type and Content-Length headers removed on redirect except for 307/308
--SKIPIF--
<?php
require_once 'server.inc';
http_server_skipif();
?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require_once 'server.inc';

$status_codes = [301, 302, 303, 307, 308];

$responses = [];
foreach ($status_codes as $code) {
    $responses[] = "data://text/plain,"
        . "HTTP/1.1 $code Redirect\r\n"
        . "Location: /$code-redirected\r\n"
        . "Content-Length: 0\r\n"
        . "\r\n";

    $responses[] = "data://text/plain,"
        . "HTTP/1.1 200 OK\r\n"
        . "Content-Length: 0\r\n"
        . "\r\n";
}

$server = http_server($responses, $output);

$context = stream_context_create([
    'http' => [
        'method' => 'POST',
        'content' => 'test=data',
        'follow_location' => 1,
        'max_redirects' => 3,
        'header' =>
            "Content-Type: application/x-www-form-urlencoded\r\n" .
            "Content-Length: 9\r\n",
    ],
]);

foreach ($status_codes as $code) {
    file_get_contents($server['uri'], false, $context);
}

http_server_kill($server['pid']);

rewind($output);
$contents = stream_get_contents($output);

foreach ($status_codes as $code) {
    if (!preg_match("~(GET|POST) /$code-redirected .*?\r\n\r\n~s", $contents, $matches)) {
        die("fail redirect request for $code not found\n");
    }
    echo "Redirect request for $code has Content-Type: ";
    var_dump(stripos($matches[0], 'content-type') !== false);
    echo "Redirect request for $code has Content-Length: ";
    var_dump(stripos($matches[0], 'content-length') !== false);
}

?>
--EXPECT--
Redirect request for 301 has Content-Type: bool(false)
Redirect request for 301 has Content-Length: bool(false)
Redirect request for 302 has Content-Type: bool(false)
Redirect request for 302 has Content-Length: bool(false)
Redirect request for 303 has Content-Type: bool(false)
Redirect request for 303 has Content-Length: bool(false)
Redirect request for 307 has Content-Type: bool(true)
Redirect request for 307 has Content-Length: bool(true)
Redirect request for 308 has Content-Type: bool(true)
Redirect request for 308 has Content-Length: bool(true)
