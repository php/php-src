--TEST--
Bug #21800 (Segfault under interactive mode) 
--SKIPIF--
<?php (PHP_SAPI != 'cli') and print "SKIP PHP binary is not cli"; ?>
--FILE--
<?php
$fh = popen("{$_ENV['TEST_PHP_EXECUTABLE']} -a", 'w');
if ($fh !== false) {
	fwrite($fh, "<?php echo ':test:'; ?>\n\n");
	fclose($fh);
} else {
	echo "failure\n";
}
?>
--EXPECT--
Interactive mode enabled

:test:
