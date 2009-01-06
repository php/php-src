--TEST--
local infile handler
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_set_local_infile_handler'))
	die("skip - function not available.");
?>
--FILE--
<?php
	include "connect.inc";

	function my_read($fp, &$buffer, $buflen, &$error) {
		$buffer = strrev(fread($fp, $buflen));
		return(strlen($buffer));
	}

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	/* create temporary file */
	$filename = dirname(__FILE__) . "061.csv";
	$fp = fopen($filename, "w");
	fwrite($fp, b"foo;bar");
	fclose($fp);

	if (!mysqli_query($link,"DROP TABLE IF EXISTS t_061"))
		printf("Cannot drop table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	if (!mysqli_query($link,"CREATE TABLE t_061 (c1 varchar(10), c2 varchar(10))"))
		printf("Cannot create table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, sprintf("LOAD DATA LOCAL INFILE '%s' INTO TABLE t_061 FIELDS TERMINATED BY ';'", mysqli_real_escape_string($link, $filename))))
		printf("Cannot load data: [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_set_local_infile_handler($link, "my_read");
	if (!mysqli_query($link, sprintf("LOAD DATA LOCAL INFILE '%s' INTO TABLE t_061 FIELDS TERMINATED BY ';'", mysqli_real_escape_string($link, $filename))))
		printf("Cannot load data using infile handler: [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if ($result = mysqli_query($link, "SELECT c1,c2 FROM t_061")) {
		while (($row = mysqli_fetch_row($result))) {
			printf("%s-%s\n", $row[0], $row[1]);
			printf("%s-%s\n", gettype($row[0]), gettype($row[1]));
		}
		mysqli_free_result($result);
	}

	mysqli_close($link);
	unlink($filename);
	print "done!";
?>
--EXPECTF--
foo-bar
unicode-unicode
rab-oof
unicode-unicode
done!
