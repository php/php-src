--TEST--
Verbose  tidy_getopt()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--INI--
tidy.default_config=
--FILE--
<?php
		$a = new tidy(__DIR__."/007.html");
		echo "Current Value of 'tidy-mark': ";
		var_dump($a->getopt("tidy-mark"));
		echo "Current Value of 'error-file': ";
		var_dump($a->getopt("error-file"));
		echo "Current Value of 'tab-size': ";
		var_dump($a->getopt("tab-size"));

		var_dump($a->getopt('bogus-opt'));
		var_dump(tidy_getopt($a, 'non-ASCII string рсч'));
?>
--EXPECTF--
Current Value of 'tidy-mark': bool(false)
Current Value of 'error-file': string(0) ""
Current Value of 'tab-size': int(8)

Warning: tidy::getOpt(): Unknown Tidy Configuration Option 'bogus-opt' in %s007.php on line 10
bool(false)

Warning: tidy_getopt(): Unknown Tidy Configuration Option 'non-ASCII string рсч' in %s007.php on line 11
bool(false)
