--TEST--
Bug #69900 Commandline input/output weird behaviour with STDIO
--FILE--
<?php

error_reporting(E_ALL);

$fl = dirname(__FILE__) . DIRECTORY_SEPARATOR . "test69900.php";

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

$process = proc_open(PHP_BINARY.' -f ' . $fl, $descriptorspec, $pipes, NULL, NULL, array("blocking_pipes" => true));

for($i = 0; $i < 10; $i++){
	fwrite($pipes[0], "hello$i\r\n");
	fflush($pipes[0]);

	$t0 = microtime(1);
	$s = fgets($pipes[1]);
	$t1 = microtime(1);

	echo $s;		
	echo "fgets() took ", (($t1 - $t0)*1000 > 1 ? 'more' : 'less'), " than 1 ms\n";
}

fclose($pipes[0]);
fclose($pipes[1]);

proc_close($process);

?>
===DONE===
--CLEAN--
<?php
$fl = dirname(__FILE__) . DIRECTORY_SEPARATOR . "test69900.php";
@unlink($fl);
?>
--EXPECT--
hello0
fgets() took more than 1 ms
hello1
fgets() took less than 1 ms
hello2
fgets() took less than 1 ms
hello3
fgets() took less than 1 ms
hello4
fgets() took less than 1 ms
hello5
fgets() took less than 1 ms
hello6
fgets() took less than 1 ms
hello7
fgets() took less than 1 ms
hello8
fgets() took less than 1 ms
hello9
fgets() took less than 1 ms
===DONE===
