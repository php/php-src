<?php

$responses = array(
    "data://text/plain,HTTP/1.0 200 Ok\r\nSome: Header1\r\n\r\nBody11",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$f = file_get_contents($uri);
var_dump($f);
var_dump($http_response_header);

http_server_kill($pid);
