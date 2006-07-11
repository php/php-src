<?php

  /* default values are localhost, root and empty password 
     Change the values if you use another configuration   */
	$driver = new mysqli_driver;

	if (!$driver->embedded) {
		$host = "localhost";
		$user = "root";
		$passwd = "";
	} else {
		$path =  dirname(__FILE__);
		$host = ":embedded";
		$user = $passwd = NULL;
		$args = array(
					"--datadir=$path", 
					"--innodb_data_home_dir=$path",
					"--innodb_data_file_path=ibdata1:10M:autoextend",
					"--log-error=$path/testrun.log",
                                        "--init-connect='CREATE DATABASE IF NOT EXISTS test;'"
				);
		$driver->embedded_server_start(TRUE, $args, NULL);
	}

?>
