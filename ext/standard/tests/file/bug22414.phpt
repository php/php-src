--TEST--
Bug #22414: passthru() does not read data correctly
--SKIPIF--
<?php
	if (ini_get("safe_mode")) {
		die('skip this test cannot be run under safe-mode');
	}
?>
--INI--
output_handler=
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$tmpfile = tempnam('/tmp', 'phpt');
	
	/* Regular Data Test */
	passthru($php . ' -r " echo \"HELLO\"; "');

	echo "\n";

	/* Binary Data Test */
	@unlink($pwd . '/passthru_test');
	
	$cmd = $php . " -r \\\" readfile(@getenv(TEST_PHP_EXECUTABLE)); \\\"";
	$cmd = $php . ' -r \' passthru("'.$cmd.'"); \' > '.$tmpfile ;
	exec($cmd);
	
	if (md5_file($php) == md5_file($tmpfile)) {
		echo "Works\n";
	} else {
		echo "Does not work\n";
	}
	
	@unlink($tmpfile);
?>
--EXPECT--
HELLO
Works
