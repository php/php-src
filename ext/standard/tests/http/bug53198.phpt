--TEST--
Bug #53198 (From: header cannot be changed with ini_set)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--INI--
allow_url_fopen=1
from=teste@teste.pt
--FILE--
<?php
require 'server.inc';

function do_test() {

    $responses = array(
        "data://text/plain,HTTP/1.0 200 OK\r\n\r\n",
    );

    $pid = http_server("tcp://127.0.0.1:12342", $responses, $output);

    foreach($responses as $r) {

        $fd = fopen('http://127.0.0.1:12342/', 'rb', false);

        fseek($output, 0, SEEK_SET);
        var_dump(stream_get_contents($output));
        fseek($output, 0, SEEK_SET);
    }

    http_server_kill($pid);

}

echo "-- Test: leave default --\n";

do_test();

echo "-- Test: after ini_set --\n";

ini_set('from', 'junk@junk.com');

do_test();

?>
--EXPECTF--
-- Test: leave default --
string(%d) "GET / HTTP/1.0
From: teste@teste.pt
Host: 127.0.0.1:12342
Connection: close

"
-- Test: after ini_set --
string(%d) "GET / HTTP/1.0
From: junk@junk.com
Host: 127.0.0.1:12342
Connection: close

"
