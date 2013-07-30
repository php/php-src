--TEST--
Bug #65069: GlobIterator fails to access files inside an open_basedir restricted dir
--FILE--
<?php
$file_path = dirname(__FILE__);
// temp dirname used here
$dirname = "$file_path/bug65069";
// temp dir created
mkdir($dirname);

ini_set('open_basedir', $dirname);

// temp files created
$fp = fopen("$dirname/wonder12345", "w");
fclose($fp);
$fp = fopen("$dirname/wonder.txt", "w");
fclose($fp);
$fp = fopen("$dirname/file.text", "w");
fclose($fp);

$spl_glob_it = new \GlobIterator("$dirname/*.txt");
foreach ($spl_glob_it as $file_info) {
	echo $file_info->getFilename() . "\n";
}

$spl_glob_it = new \GlobIterator(dirname(dirname($dirname)) . "/*/*/*");
foreach ($spl_glob_it as $file_info) {
	echo $file_info->getFilename() . "\n";
}

$spl_glob_empty = new \GlobIterator("$dirname/*.php");
try {
	$spl_glob_empty->count();
} catch (LogicException $e) {
	echo "logic exception\n";
}

try {
	$spl_glob_not_allowed = new \GlobIterator(dirname(dirname($dirname)));
} catch (Exception $e) {
	echo "exception1\n";
}

try {
	$spl_glob_sec = new \GlobIterator("$file_path/bug65069-this-will-never-exists");
} catch (Exception $e) {
	echo "exception2\n";
}

?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$dirname = "$file_path/bug65069";
unlink("$dirname/wonder12345");
unlink("$dirname/wonder.txt");
unlink("$dirname/file.text");
rmdir($dirname);
?>
--EXPECT--
wonder.txt
file.text
wonder.txt
wonder12345
logic exception
exception1
exception2