--TEST--
Bug #22414: passthru() does not read data correctly
--SKIPIF--
<?php if (strtolower(@getenv('TEST_PHP_EXECUTABLE'))=='auto') die('skip: TEST_PHP_EXECUTABLE = auto'); ?>
--INI--
safe_mode=
output_handler=
--FILE--
<?php

	$php = getenv('TEST_PHP_EXECUTABLE');
	$tmpfile = tempnam('/tmp', 'phpt');
	
	/* Regular Data Test */
	passthru($php . ' -n -r " echo \"HELLO\"; "');

	echo "\n";

	/* Binary Data Test */
	
	$cmd = $php . ' -n -r \"readfile(@getenv(\'TEST_PHP_EXECUTABLE\')); \"';
	$cmd = $php . ' -n -r \' passthru("'.$cmd.'"); \' > '.$tmpfile ;
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
