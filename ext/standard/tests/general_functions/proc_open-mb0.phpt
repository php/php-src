--TEST--
proc_open with bypass_shell subprocess parameter passing
--SKIPIF--
<?php
if (php_sapi_name() != "cli") die('skip CLI only test');
if (!function_exists("proc_open")) echo "skip proc_open() is not available";
?>
--FILE--
<?php

$php = PHP_BINARY;

$f = __DIR__ . DIRECTORY_SEPARATOR . "proc_only_mb0.php";
file_put_contents($f,'<?php var_dump($argv); ?>');

$ds = array(
		0 => array("pipe", "r"),
		1 => array("pipe", "w"),
		2 => array("pipe", "w")
		);

$p = proc_open(
		"$php -n $f テストマルチバイト・パス füße карамба",
		$ds,
		$pipes,
		NULL,
		NULL,
		array("bypass_shell" => true)
		);

$out = "";

while (!feof($pipes[1])) {
	$out .= fread($pipes[1], 1024);
}

proc_close($p);

echo $out;

?>
==DONE==
--EXPECTF--
array(4) {
  [0]=>
  string(%d) "%sproc_only_mb0.php"
  [1]=>
  string(36) "テストマルチバイト・パス"
  [2]=>
  string(6) "füße"
  [3]=>
  string(14) "карамба"
}
==DONE==
