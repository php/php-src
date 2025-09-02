--TEST--
GH-15653 (fgetcsv overflow on length argument)
--FILE--
<?php
$filename = __DIR__ . "/gh15653.tmp";
touch($filename);
$fp = fopen ($filename, "r");

try {
	fgetcsv($fp, PHP_INT_MAX, escape: '');
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

fgetcsv($fp, PHP_INT_MAX-1,  escape: '');
--CLEAN--
<?php
@unlink(__DIR__ . "/gh15653.tmp");
?>
--EXPECTF--
fgetcsv(): Argument #2 ($length) must be between 0 and %d
%A
