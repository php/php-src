<?php

  /* default values are localhost, root and empty password 
     Change the MYSQL_TEST environment values if you want to 
     use another configuration   */
	$driver = new mysqli_driver;

	$host = getenv("MYSQL_TEST_HOST") ? getenv("MYSQL_TEST_HOST") : "localhost";
	$port = getenv("MYSQL_TEST_PORT") ? getenv("MYSQL_TEST_PORT") : 3306;
	$user = getenv("MYSQL_TEST_USER") ? getenv("MYSQL_TEST_USER") : "root";
	$passwd = getenv("MYSQL_TEST_PASSWD") ? getenv("MYSQL_TEST_PASSWD") : "";
	$db = getenv("MYSQL_TEST_DB") ? getenv("MYSQL_TEST_DB") : "phptest";
	$engine = getenv("MYSQL_TEST_ENGINE") ? getenv("MYSQL_TEST_ENGINE") : "MyISAM";
?>
