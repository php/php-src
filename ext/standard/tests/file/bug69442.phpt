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

    exec($_SERVER['TEST_PHP_EXECUTABLE']." -d display_errors=1 -d error_reporting=E_ALL ".$tmpFile." 2>&1", $output);
    $output = join("\n", $output);
    unlink($tmpFile);

    if (strstr($output, "PTY (pseudoterminal) not supported on this system") !== false) {
        die("skip PTY pseudo terminals are not supported");
    }
--FILE--
<?php
$cmd = '(echo "foo" ; exit 42;) 3>/dev/null; code=$?; echo $code >&3; exit $code';
$descriptors = array(array("pty"), array("pty"), array("pty"), array("pipe", "w"));
$pipes = array();

$process = proc_open($cmd, $descriptors, $pipes);

function read_from_pipe($pipe) {
    $result = fread($pipe, 1000);
    /* We can't guarantee that everything written to the pipe will be returned by a single call
     *   to fread(), even if it was written with a single syscall and the number of bytes written
     *   was small */
    $again  = @fread($pipe, 1000);
    if ($again) {
        $result .= $again;
    }
    return $result;
}

$data0 = read_from_pipe($pipes[0]);
echo 'read from pipe 0: ';
var_dump($data0);
fclose($pipes[0]);

$data3 = read_from_pipe($pipes[3]);
echo 'read from pipe 3: ';
var_dump($data3);
fclose($pipes[3]);

proc_close($process);
?>
--EXPECT--
read from pipe 0: string(5) "foo
"
read from pipe 3: string(3) "42
"
