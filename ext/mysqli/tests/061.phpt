--TEST--
local infile handler
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	function my_read($fp, &$buffer, $buflen, &$error) {
		$buffer = strrev(fread($fp, $buflen));
		return(strlen($buffer));
	}
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd, "test");

	/* create temporary file */
	$fp = fopen("061.csv", "w");
	fwrite($fp, "foo;bar");
	fclose($fp);

  	mysqli_query($link,"DROP TABLE IF EXISTS t_061");
  	mysqli_query($link,"CREATE TABLE t_061 (c1 varchar(10), c2 varchar(10))");

	mysqli_query($link, "LOAD DATA LOCAL INFILE '061.csv' INTO TABLE t_061 FIELDS TERMINATED BY ';'"); 

	mysqli_set_local_infile_handler($link, "my_read");
	mysqli_query($link, "LOAD DATA LOCAL INFILE '061.csv' INTO TABLE t_061 FIELDS TERMINATED BY ';'"); 

	if ($result = mysqli_query($link, "SELECT c1,c2 FROM t_061")) {
		while (($row = mysqli_fetch_row($result))) {
			printf("%s-%s\n", $row[0], $row[1]);
		}
		mysqli_free_result($result);
	}

	mysqli_close($link);
?>
--EXPECT--
foo-bar
rab-oof
