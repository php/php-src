--TEST--
Bug #22414 (passthru() does not read data correctly)
--INI--
output_handler=
--FILE--
<?php

	$php = getenv('TEST_PHP_EXECUTABLE');
	$tmpfile = tempnam(__DIR__, 'phpt');
	$args = ' -n -dsafe_mode=off ';

	/* Regular Data Test */
	passthru($php . $args . ' -r " echo \"HELLO\"; "');

	echo "\n";

	/* Binary Data Test */

	if (substr(PHP_OS, 0, 3) != 'WIN') {
		$cmd = $php . $args . ' -r \"readfile(@getenv(\'TEST_PHP_EXECUTABLE\')); \"';
		$cmd = $php . $args . ' -r \' passthru("'.$cmd.'"); \' > '.$tmpfile ;
	} else {
		$cmd = $php . $args . ' -r \"readfile(@getenv(\\\\\\"TEST_PHP_EXECUTABLE\\\\\\")); \"';
		$cmd = $php . $args . ' -r " passthru(\''.$cmd.'\');" > '.$tmpfile ;
	}
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
