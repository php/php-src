--TEST--
Verbose  tidy_getopt()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--INI--
tidy.default_config=
unicode.script_encoding=latin1
--FILE--
<?php
		$a = new tidy(dirname(__FILE__)."/007.html");
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
Current Value of 'error-file': unicode(0) ""
Current Value of 'tab-size': int(8)

Warning: tidy::getOpt(): Unknown Tidy Configuration Option 'bogus-opt' in %s007.php on line 10
bool(false)

Warning: Could not convert Unicode string to binary string (converter US-ASCII failed on character {U+00E0} at offset 17) in %s007.php on line 11
bool(false)
