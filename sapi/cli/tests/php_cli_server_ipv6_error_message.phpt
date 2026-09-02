--TEST--
IPv6 address error message formatting
--SKIPIF--
<?php
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

$php = getenv('TEST_PHP_EXECUTABLE');
$args = getenv('TEST_PHP_EXTRA_ARGS');
$cmd = "$php $args" . ' -S "[2001:db8::]:8080"';

$process = proc_open(
    $cmd,
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
string(%d) "[%s] Failed to listen on [2001:db8::]:8080 %s
"
