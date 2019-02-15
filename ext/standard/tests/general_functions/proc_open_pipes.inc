<?php

function create_sleep_script()
{
	$fname = dirname(__FILE__) . DIRECTORY_SEPARATOR . "proc_open_pipes_sleep.php";

	if (!file_exists($fname)) {
		file_put_contents($fname, "<?php\nsleep(1);\n");
	}

	return $fname;
}

function unlink_sleep_script()
{
	$fname = dirname(__FILE__) . DIRECTORY_SEPARATOR . "proc_open_pipes_sleep.php";

	if (file_exists($fname)) {
		unlink($fname);
	}
}
