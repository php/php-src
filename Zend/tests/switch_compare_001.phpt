--TEST--
Test switch with all types of variables
--FILE--
<?php
function foo($var) {
	switch ($var) {
		case null:
			print "a";
		case false:
			print "b";
		case true:
			print "c";
		case "a":
			print "d";
		case 0:
			print "e";
		default:
			print "f";
	}
	print "\n";
}

foo("a");
foo("0");
foo(1.2);
foo(true);
foo(false);

print "\n";

function bar($var) {
	switch ($var) {
		case "0e1":
			print "a";
		case 0:
			print "b";
		case true:
			print "c";
		default:
			print "d";
	}
	print "\n";
}

bar(0);
bar("0");
bar("0x0");
bar(false);

print "\n";

?>
--EXPECT--
cdef
bcdef
cdef
cdef
abcdef

abcd
abcd
abcd
bcd
