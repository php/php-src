--TEST--
Bug #65069: GlobIterator fails to access files inside an open_basedir restricted dir
--FILE--
<?php
ini_set('open_basedir', __DIR__);

$path = (dirname(__DIR__) . '/*/*');
$std_glob = glob($path);
$spl_glob = array();
try {
	$spl_glob_it = new \GlobIterator($path);
	foreach ($spl_glob_it as $file_info) {
		$spl_glob[] = $file_info->getPathname();
	}
} catch (Exception $e) {
	var_dump($e->getMessage());
}
if (!empty($std_glob))
	echo $std_glob == $spl_glob ? "SUCCESS" : "FAILURE";
?>
--EXPECT--
SUCCESS
