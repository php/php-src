--TEST--
dirname test
--FILE--
<?php

	function check_dirname($path)
	{
		print "dirname($path) == " . dirname($path) . "\n";
	}
	
	check_dirname("/foo/");
	check_dirname("/foo");
	check_dirname("/foo/bar");
	check_dirname("d:\\foo\\bar.inc");
	check_dirname("/");
	check_dirname(".../foo");
	check_dirname("./foo");
	check_dirname("foobar///");
	check_dirname("c:\foo");
?>
--EXPECT--
dirname(/foo/) == /
dirname(/foo) == /
dirname(/foo/bar) == /foo
dirname(d:\foo\bar.inc) == .
dirname(/) == /
dirname(.../foo) == ...
dirname(./foo) == .
dirname(foobar///) == .
dirname(c:\foo) == .
