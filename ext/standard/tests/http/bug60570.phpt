--TEST--
Bug #60570 (Stream context leaks when http request fails)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

function do_test() {

    $responses = array(
        "data://text/plain,HTTP/1.0 404 Not Found\r\n\r\n",
        "data://text/plain,HTTP/1.0 404 Not Found\r\n\r\n",
        "data://text/plain,HTTP/1.0 404 Not Found\r\n\r\n"
    );

    ['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

    $a = $b = count(get_resources());

    $i = 3;
    while ($i--) {
        $context = stream_context_create(array('http'=>array('timeout'=>1)));
        file_get_contents($uri, 0, $context);
        unset($context);

        $b = $a;
        $a = count(get_resources());
    }

    http_server_kill($pid);

    echo "leak? penultimate iteration: $b, last one: $a\n";
    var_dump($a == $b);
}

do_test();
?>
--EXPECTF--
Warning: file_get_contents(http://%s:%d): Failed to open stream: HTTP request failed! HTTP/1.0 404 Not Found
 in %s on line %d

Warning: file_get_contents(http://%s:%d): Failed to open stream: HTTP request failed! HTTP/1.0 404 Not Found
 in %s on line %d

Warning: file_get_contents(http://%s:%d): Failed to open stream: HTTP request failed! HTTP/1.0 404 Not Found
 in %s on line %d
leak? penultimate iteration: %d, last one: %d
bool(true)
