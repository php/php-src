--TEST--
proc_open with PTY closes incorrect file descriptor
--SKIPIF--
<?php

$code = <<< 'EOC'
    <?php
    $descriptors = array(array("pty"), array("pty"), array("pty"), array("pipe", "w"));
    $pipes = array();
    $process = proc_open('echo "foo";', $descriptors, $pipes);
EOC;

    $tmpFile = tempnam(sys_get_temp_dir(), "bug69442");
    file_put_contents($tmpFile, $code);

    exec($_SERVER['TEST_PHP_EXECUTABLE']." ".$tmpFile." 2>&1", $output);
    $output = join("\n", $output);
    unlink($tmpFile);

    if (strstr($output, "pty pseudo terminal not supported on this system") !== false) {
        die("skip PTY pseudo terminals are not supported");
    }
--FILE--
<?php
$cmd = '(echo "foo" ; exit 42;) 3>/dev/null; code=$?; echo $code >&3; exit $code';
$descriptors = array(array("pty"), array("pty"), array("pty"), array("pipe", "w"));
$pipes = array();

$process = proc_open($cmd, $descriptors, $pipes);

foreach ($pipes as $type => $pipe) {
    $data = fread($pipe, 999);
    echo 'type ' . $type . ' ';
    var_dump($data);
    fclose($pipe);
}
proc_close($process);
--EXPECT--
type 0 string(5) "foo
"
type 1 string(0) ""
type 2 string(0) ""
type 3 string(3) "42
"


