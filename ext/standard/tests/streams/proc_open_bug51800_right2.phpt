--TEST--
Bug #51800 proc_open on Windows hangs forever, the right way to do it with more data
--FILE--
<?php
$callee = __DIR__ . "/process_proc_open_bug51800_right2.php";
$php = PHP_BINARY;
$cmd = "$php -n $callee";

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
$how_much = 1000000;

$data0 = str_repeat("a", $how_much);
$data1 = str_repeat("b", $how_much);
$i0 = $i1 = 0;
$step = 1024;

while ($i0 < strlen($data0) && $i1 < strlen($data1)) {
	fwrite(STDOUT, substr($data0, $i0, $step));
	fwrite(STDERR, substr($data1, $i1, $step));
	$i0 += $step;
	$i1 += $step;
}

exit(0);
');

if (!$r) {
	die("couldn't create helper script '$callee'");
}

$process = proc_open($cmd, $descriptors, $pipes);

if (is_resource($process))
{
        fclose($pipes[0]);

        while (!feof($pipes[1]) || !feof($pipes[2])) {
                $stdout .= fread($pipes[1], 1024);
                $stderr .= fread($pipes[2], 1024);
        }
        fclose($pipes[1]);
        fclose($pipes[2]);

        $status = proc_close($process);
}

var_dump(array(
        "status" => $status,
        "stdout" => $stdout,
        "stderr" => $stderr,
), strlen($stdout), strlen($stderr));

?>
===DONE===
--CLEAN--
<?php
$callee = __DIR__ . "/process_proc_open_bug51800_right2.php";
unlink($callee);
?>
--EXPECTF--
array(3) {
  ["status"]=>
  int(0)
  ["stdout"]=>
  string(1000000) "a%s"
  ["stderr"]=>
  string(1000000) "b%s"
}
int(1000000)
int(1000000)
===DONE===
