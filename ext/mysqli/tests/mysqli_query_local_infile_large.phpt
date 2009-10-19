--TEST--
mysql_query(LOAD DATA LOCAL INFILE) with large data set (10MB)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysqli.allow_local_infile=1
--FILE--
<?php
	// Create a large CVS file
	$file = tempnam(sys_get_temp_dir(), 'mysqli_test.cvs');
	if (!$fp = fopen($file, 'w'))
		printf("[001] Cannot create CVS file '%s'\n", $file);

	$data = str_repeat("a", 127) . ";" . str_repeat("b", 127) . "\n";

	$runtime = 5;
	$max_bytes = 1024 * 1024 * 10;

	$start = microtime(true);
	$bytes = 0;
	$rowno = 0;
	while (($bytes < $max_bytes) && ((microtime(true) - $start) < $runtime)) {
		if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1))
			$bytes += fwrite($fp, (binary)(++$rowno . ";" . $data));
		else
			$bytes += fwrite($fp, ++$rowno . ";" . $data);
	}
	fclose($fp);
	printf("Filesize in bytes: %d\nRows: %d\n", $bytes, $rowno);

	require_once("connect.inc");
 	if (!($link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)))
		printf("[002] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	if (!mysqli_query($link, "DROP TABLE IF EXISTS test") ||
			!mysqli_query($link, "CREATE TABLE test(id INT, col1 VARCHAR(255), col2 VARCHAR(255)) ENGINE = " . $engine))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, sprintf("LOAD DATA LOCAL INFILE '%s' INTO TABLE test FIELDS TERMINATED BY ';'", mysqli_real_escape_string($link, $file))))
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if ((!is_string(mysqli_info($link))) || ('' == mysqli_info($link))) {
		printf("[005] [%d] %s, mysqli_info not set \n", mysqli_errno($link), mysqli_error($link));
	}

	if (!($res = mysqli_query($link, "SELECT COUNT(*) AS _num FROM test")))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$row = mysqli_fetch_assoc($res);
	if (($row["_num"] != $rowno))
		printf("[007] Expecting %d rows, found %d\n", $rowno, $row["_num"]);

	mysqli_free_result($res);

	$random = mt_rand(1, $rowno);
	if (!$res = mysqli_query($link, "SELECT id, col1, col2 FROM test WHERE id = " . $random))
			printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$row = mysqli_fetch_assoc($res);
	var_dump($row);
	mysqli_free_result($res);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
$file = tempnam(sys_get_temp_dir(), 'mysqli_test.cvs');
if (file_exists($file))
	unlink($file);

require_once("connect.inc");
if (!($link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)))
	printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
	printf("[c002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
?>
--EXPECTF--
Filesize in bytes: %d
Rows: %d
array(3) {
  [%u|b%"id"]=>
  %unicode|string%(%d) "%d"
  [%u|b%"col1"]=>
  %unicode|string%(127) "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
  [%u|b%"col2"]=>
  %unicode|string%(127) "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
}
done!