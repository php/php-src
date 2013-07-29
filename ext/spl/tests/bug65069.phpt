--TEST--
Bug #65069: GlobIterator fails to access files inside an open_basedir restricted dir
--FILE--
<?php
$file_path = dirname(__FILE__);
ini_set('open_basedir', $file_path);

// temp dirname used here
$dirname = "$file_path/bug65069";

// temp dir created
mkdir($dirname);

// temp files created
$fp = fopen("$dirname/wonder12345", "w");
fclose($fp);
$fp = fopen("$dirname/wonder.txt", "w");
fclose($fp);
$fp = fopen("$dirname/file.text", "w");
fclose($fp);

$pattern = "$dirname/*.txt";

try {
	$spl_glob_it = new \GlobIterator($pattern);
	foreach ($spl_glob_it as $file_info) {
		$spl_glob[] = $file_info->getFilename();
	}
} catch (Exception $e) {
	var_dump($e->getMessage());
}

if (count($spl_glob) == 1)
	echo $spl_glob[0]
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
