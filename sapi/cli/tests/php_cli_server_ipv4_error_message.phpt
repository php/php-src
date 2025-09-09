--TEST--
IPv4 address error message formatting
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) {
    die("skip online test");
}
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die("skip not for Windows");
}
?>
--FILE--
<?php

$descriptorspec = array(
    0 => array("pipe", "r"),
    1 => array("pipe", "w"),  
    2 => array("pipe", "w")
);

$process = proc_open(
    PHP_BINARY . ' -S "192.168.1.999:8080"',
    $descriptorspec,
    $pipes
);

if (is_resource($process)) {
    usleep(100000);
    
    $stderr = stream_get_contents($pipes[2]);
    
    fclose($pipes[0]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    
    proc_terminate($process);
    proc_close($process);

    var_dump($stderr);
}
?>
--EXPECTF--
string(%d) "[%s] Failed to listen on 192.168.1.999:8080 %s
"
