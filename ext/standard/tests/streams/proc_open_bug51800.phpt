--TEST--
Bug #51800 proc_open on Windows hangs forever
--SKIPIF--
<?php
    echo 'skip expected to fail or take too long';
    if (getenv("SKIP_SLOW_TESTS")) {
        die("skip slow test");
    }
?>
--XFAIL--
pipes have to be read/written simultaneously
--FILE--
<?php
/* This is the wrong way to do it. The parent will block till it has read all the STDIN.
The smaller the pipe buffer is, the longer it will take. It might even pass at the end,
after taking inappropriately long. Pipes have to be read simultaneously in smaller chunks,
so then the pipe buffer is emptied more often and the child has chance to continue its
write. The behaviour might look some better if write/read in a separate thread, however
this is much more resource greedy and complexer to integrate into the user script. */

$callee = __DIR__ . "/process_proc_open_bug51800.php";
$php = PHP_BINARY;
$cmd = "$php $callee";

$status;
$stdout = "";
$stderr = "";
$pipes = array();

$descriptors = array(
        0 => array("pipe", "rb"),       // stdin
        1 => array("pipe", "wb"),       // stdout
        2 => array("pipe", "wb")                // stderr
        );

/* create the proc file */
$r = file_put_contents($callee, '<?php

$how_much = 10000;

$data0 = str_repeat("a", $how_much);
$data1 = str_repeat("b", $how_much);
fwrite(STDOUT, $data0);
fwrite(STDERR, $data1);

exit(0);
');

if (!$r) {
    die("couldn't create helper script '$callee'");
}

$process = proc_open($cmd, $descriptors, $pipes);

if (is_resource($process))
{
        fclose($pipes[0]);

    while (!feof($pipes[1]))
        $stdout .= fread($pipes[1], 1024);
    fclose($pipes[1]);

    while (!feof($pipes[2]))
        $stderr .= fread($pipes[2], 1024);
    fclose($pipes[2]);

        $status = proc_close($process);
}

var_dump(array(
        "status" => $status,
        "stdout" => $stdout,
        "stderr" => $stderr,
), strlen($stdout), strlen($stderr));

?>
--CLEAN--
<?php
$callee = __DIR__ . "/process_proc_open_bug51800.php";
unlink($callee);
?>
--EXPECTF--
array(3) {
  ["status"]=>
  int(0)
  ["stdout"]=>
  string(10000) "a%s"
  ["stderr"]=>
  string(10000) "b%s"
}
int(10000)
int(10000)
