--TEST--
Bug #69900 Commandline input/output weird behaviour with STDIO
--CONFLICTS--
all
--FILE--
<?php

error_reporting(E_ALL);

$fl = __DIR__ . DIRECTORY_SEPARATOR . "test69900.php";
$max_ms = 20;

$test_content = '<?php

$in = fopen("php://stdin", "rb", false, stream_context_create(array("pipe" => array("blocking" => true))));

while(!feof($in)){
$s = fgets($in);
    fwrite(STDOUT, $s);
}

?>';
file_put_contents($fl, $test_content);

$descriptorspec = array(0 => array("pipe", "r"),1 => array("pipe", "w"));
$pipes = array();

$process = proc_open(getenv('TEST_PHP_EXECUTABLE_ESCAPED').' -n -f ' . escapeshellarg($fl), $descriptorspec, $pipes, NULL, NULL, array("blocking_pipes" => true));

$moreThanLimit = 0;
for($i = 0; $i < 10; $i++){
    fwrite($pipes[0], "hello$i\r\n");
    fflush($pipes[0]);

    $t0 = microtime(1);
    $s = fgets($pipes[1]);
    $t1 = microtime(1);

    echo $s;

    $dt_ms = ($t1 - $t0)*1000;
    if ($dt_ms > $max_ms) {
        $moreThanLimit++;
    }
}

fclose($pipes[0]);
fclose($pipes[1]);

proc_close($process);

/* It is expected that the first call takes more than the limit.
 * Allow two more to account for possible process switch, etc. */
if ($moreThanLimit > 3) {
    echo "fgets() took more than $max_ms ms $moreThanLimit times\n";
}

?>
--CLEAN--
<?php
$fl = __DIR__ . DIRECTORY_SEPARATOR . "test69900.php";
@unlink($fl);
?>
--EXPECT--
hello0
hello1
hello2
hello3
hello4
hello5
hello6
hello7
hello8
hello9
