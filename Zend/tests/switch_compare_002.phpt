--TEST--
Test switch with multiple string cases
--FILE--
<?php
function foo ($var) {
	switch ($var) {
		case "a":
			print "a";
		case "b":
			print "b";
		case 0:
			print "c";
		default:
			print "d";
		case "1":
			print "e";
		case "c":
			print "f";
	}
	print "\n";
}

foo("a");
foo("b");
foo("c");
foo("1");
foo(0);
foo(1);
foo(false);
foo(null);
?>
--EXPECT--
abcdef
bcdef
cdef
ef
abcdef
ef
cdef
cdef
