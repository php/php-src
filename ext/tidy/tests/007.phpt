--TEST--
Verbose  tidy_getopt()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--INI--
tidy.default_config=
--FILE--
<?php
		$a = new tidy(dirname(__FILE__)."/007.html");
		echo "Current Value of 'tidy-mark': ";
		var_dump($a->getopt("tidy-mark"));
		echo "Current Value of 'error-file': ";
		var_dump($a->getopt("error-file"));
		echo "Current Value of 'tab-size': ";
		var_dump($a->getopt("tab-size"));
		
?>
--EXPECT--
Current Value of 'tidy-mark': bool(false)
Current Value of 'error-file': string(0) ""
Current Value of 'tab-size': int(8)
