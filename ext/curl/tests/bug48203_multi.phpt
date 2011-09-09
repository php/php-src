--TEST--
Variation of bug #48203 with curl_multi_exec (Crash when file pointers passed to curl are closed before calling curl_multi_exec)
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

function checkForClosedFilePointer($curl_option, $description) {
	$fp = fopen(dirname(__FILE__) . '/bug48203.tmp', 'w');

	$ch1 = curl_init();
	$ch2 = curl_init();

	$options = array(
		CURLOPT_RETURNTRANSFER => 1,
		$curl_option => $fp,
		CURLOPT_URL => getenv("PHP_CURL_HTTP_REMOTE_SERVER")
	);

	// we also need to set CURLOPT_VERBOSE to test CURLOPT_STDERR properly
	if (CURLOPT_STDERR == $curl_option) {
		$options[CURLOPT_VERBOSE] = 1;
	}

	if (CURLOPT_INFILE == $curl_option) {
	    $options[CURLOPT_UPLOAD] = 1;
	}

	curl_setopt_array($ch1, $options);
	curl_setopt_array($ch2, $options);

	fclose($fp); // <-- premature close of $fp caused a crash!

	$mh = curl_multi_init();

	curl_multi_add_handle($mh, $ch1);
	curl_multi_add_handle($mh, $ch2);

	$active = 0;
	do {
		curl_multi_exec($mh, $active);
	} while ($active > 0);

	curl_multi_remove_handle($mh, $ch1);
	curl_multi_remove_handle($mh, $ch2);
	curl_multi_close($mh);

	echo "Ok for $description\n";
}

$options_to_check = array(
	"CURLOPT_STDERR", "CURLOPT_WRITEHEADER", "CURLOPT_FILE", "CURLOPT_INFILE"
);

foreach($options_to_check as $option) {
	checkForClosedFilePointer(constant($option), $option);
}

?>
--CLEAN--
<?php @unlink(dirname(__FILE__) . '/bug48203.tmp'); ?>
--EXPECTF--
Warning: curl_multi_exec(): %d is not a valid stream resource in %s on line %d

Warning: curl_multi_exec(): CURLOPT_STDERR handle is incorrect in %s on line %d

Warning: curl_multi_exec(): %d is not a valid stream resource in %s on line %d

Warning: curl_multi_exec(): CURLOPT_STDERR handle is incorrect in %s on line %d
%a
Ok for CURLOPT_STDERR

Warning: curl_multi_exec(): %d is not a valid stream resource in %s on line %d

Warning: curl_multi_exec(): CURLOPT_WRITEHEADER handle is incorrect in %s on line %d

Warning: curl_multi_exec(): %d is not a valid stream resource in %s on line %d

Warning: curl_multi_exec(): CURLOPT_WRITEHEADER handle is incorrect in %s on line %d
%AOk for CURLOPT_WRITEHEADER

Warning: curl_multi_exec(): %d is not a valid stream resource in %s on line %d

Warning: curl_multi_exec(): CURLOPT_FILE handle is incorrect in %s on line %d

Warning: curl_multi_exec(): %d is not a valid stream resource in %s on line %d

Warning: curl_multi_exec(): CURLOPT_FILE handle is incorrect in %s on line %d
%a
Ok for CURLOPT_FILE

Warning: curl_multi_exec(): %d is not a valid stream resource in %s on line %d

Warning: curl_multi_exec(): CURLOPT_INFILE handle is incorrect in %s on line %d

Warning: curl_multi_exec(): %d is not a valid stream resource in %s on line %d

Warning: curl_multi_exec(): CURLOPT_INFILE handle is incorrect in %s on line %d
Ok for CURLOPT_INFILE
