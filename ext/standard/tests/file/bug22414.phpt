--TEST--
Bug #22414: passthru() does not read data correctly
--SKIPIF--
<?php
	$cat_path = @shell_exec("which cat");
	if (empty($cat_path)) {
		die('skip cat binary needed for this test is not avaliable');	
	}
?>
--POST--
--GET--
--FILE--
<?php
	$php = getenv('TEST_PHP_EXECUTABLE');
	$pwd = realpath(dirname(__FILE__));
	
	/* Regular Data Test */
	passthru($php . ' -r " echo \"HELLO\"; "');

	echo "\n";

	/* Binary Data Test */
	@unlink($pwd . '/passthru_test');
	
	$cmd = $php . ' -r \' passthru("cat ' . $php . '"); \' > ' . $pwd . '/passthru_test';
	exec($cmd);
	
	if (md5_file($php) == md5_file($pwd . '/passthru_test')) {
		echo "Works\n";
	} else {
		echo "Does not work\n";
	}
	
	@unlink($pwd . '/passthru_test');
?>
--EXPECT--
HELLO
Works
