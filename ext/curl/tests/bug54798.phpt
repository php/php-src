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
--CLEAN--
<?php @unlink(dirname(__FILE__) . '/bug54798.tmp'); ?>
--EXPECTF--
Warning: curl_exec(): CURLOPT_STDERR resource has gone away, resetting to stderr in %sbug54798.php on line %d
* About to connect() %a
* Closing connection #%d
Ok for CURLOPT_STDERR

Warning: curl_exec(): CURLOPT_WRITEHEADER resource has gone away, resetting to default in %sbug54798.php on line 24
Ok for CURLOPT_WRITEHEADER

Warning: curl_exec(): CURLOPT_FILE resource has gone away, resetting to default in %sbug54798.php on line 24
%a
Ok for CURLOPT_FILE

Warning: curl_exec(): CURLOPT_INFILE resource has gone away, resetting to default in %sbug54798.php on line %d
Ok for CURLOPT_INFILE
