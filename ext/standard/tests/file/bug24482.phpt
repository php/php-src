--TEST--
Bug #24482 (GLOB_ONLYDIR not working)
--SKIPIF--
<?php
if (!function_exists("glob")) {
    die('skip glob() not available');
}
?>
--FILE--
<?php
$globdirs = glob("*", GLOB_ONLYDIR);

$dirs = array();
$dh = opendir(".");
while (is_string($file = readdir($dh))) {
	if ($file[0] === ".") continue;
	if (!is_dir($file)) continue;
	$dirs[] = $file;
}
closedir($dh);

if (count($dirs) != count($globdirs)) {
	echo "Directory count mismatch\n";
	
	echo "glob found:\n";
	sort($globdirs);	
	var_dump($globdirs);
	
	echo "opendir/readdir/isdir found:\n";
	sort($dirs);	
	var_dump($dirs);
} else {
	echo "OK\n";
}
?>
--EXPECT--
OK
