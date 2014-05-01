--TEST--
Bug #54798 (Segfault when CURLOPT_STDERR file pointer is closed before calling curl_exec)
--SKIPIF--
<?php 
if (!extension_loaded("curl")) {
	exit("skip curl extension not loaded");
}
if (false === getenv('PHP_CURL_HTTP_REMOTE_SERVER'))  {
	exit("skip PHP_CURL_HTTP_REMOTE_SERVER env variable is not defined");
}
?>
--FILE--
<?php

function checkForClosedFilePointer($host, $curl_option, $description) {
	$fp = fopen(dirname(__FILE__) . '/bug54798.tmp', 'w+');

	$ch = curl_init();

	// we also need CURLOPT_VERBOSE to be set to test CURLOPT_STDERR properly
	if (CURLOPT_STDERR == $curl_option) {
		curl_setopt($ch, CURLOPT_VERBOSE, 1);
	}

    if (CURLOPT_INFILE == $curl_option) {
        curl_setopt($ch, CURLOPT_UPLOAD, 1);
    }

	curl_setopt($ch, $curl_option, $fp);
	
	curl_setopt($ch, CURLOPT_URL, $host);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

	fclose($fp); // <-- premature close of $fp caused a crash!

	curl_exec($ch);

	curl_close($ch);

	echo "Ok for $description\n";
}

$options_to_check = array(
	"CURLOPT_STDERR",
    "CURLOPT_WRITEHEADER",
    "CURLOPT_FILE",
    "CURLOPT_INFILE"
);

$host = getenv('PHP_CURL_HTTP_REMOTE_SERVER');
foreach($options_to_check as $option) {
	checkForClosedFilePointer($host, constant($option), $option);
}

?>
===DONE===
--CLEAN--
<?php @unlink(dirname(__FILE__) . '/bug54798.tmp'); ?>
--EXPECTF--
%a
%aOk for CURLOPT_STDERR
%aOk for CURLOPT_WRITEHEADER
%aOk for CURLOPT_FILE
%aOk for CURLOPT_INFILE
===DONE===
