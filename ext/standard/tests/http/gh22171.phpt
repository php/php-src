--TEST--
GH-22171 (http(s) stream wrapper sends a corrupted Authorization header for percent-encoded userinfo)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

function probe(string $label, string $userinfo, string $expected): void
{
    $responses = [
        "data://text/plain,HTTP/1.0 200 Ok\r\n\r\n",
    ];

    ['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

    $url = preg_replace('(^http://)', 'http://' . $userinfo . '@', $uri);
    file_get_contents($url);

    fseek($output, 0, SEEK_SET);
    $output = stream_get_contents($output);

    http_server_kill($pid);

    if (preg_match('(^Authorization:\s*Basic\s+(\S+))mi', $output, $m)) {
        $decoded = base64_decode($m[1]);
    } else {
        $decoded = '<no Authorization header>';
    }

    echo "=== {$label} ===", PHP_EOL;
    echo "  decoded : ", addcslashes($decoded, "\0..\37"), PHP_EOL;
    echo "  result  : ", ($decoded === $expected ? "OK" : "CORRUPT"), PHP_EOL;
}

probe('user only', '%76%6f%72%74%66%75', 'vortfu:');
probe('user + password', '%76%6f%72%74%66%75:%70%61%73%73%77%6f%72%64', 'vortfu:password');
?>
--EXPECT--
=== user only ===
  decoded : vortfu:
  result  : OK
=== user + password ===
  decoded : vortfu:password
  result  : OK
