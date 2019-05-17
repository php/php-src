--TEST--
Bug #49847 (exec() fails on lines larger then 4095 bytes)
--FILE--
<?php
$iswin =  substr(PHP_OS, 0, 3) == "WIN";

if ($iswin) {
	$f = __DIR__ . '\\bug49847.tmp';
	$s = str_repeat(' ', 4097);
	$s .= '1';
	file_put_contents($f, $s);
	exec('type ' . $f, $output);
} else {
	exec("printf %4098d 1", $output);
}
var_dump($output);
if ($iswin) {
	unlink($f);
}
?>
--EXPECTF--
array(1) {
  [0]=>
  string(4098) "%s 1"
}
