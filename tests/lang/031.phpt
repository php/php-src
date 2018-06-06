--TEST--
Bug #16227 (Internal hash position bug on assignment)
--FILE--
<?php
// reported by php.net@alienbill.com
$arrayOuter = array("key1","key2");
$arrayInner = array("0","1");

print "Correct - with inner loop reset.\n";

while(list(,$o) = each($arrayOuter)){
	reset($arrayInner);
	while(list(,$i) = each($arrayInner)){
	    	print "inloop $i for $o\n";
	}
}
reset($arrayOuter);
reset($arrayInner);

print "What happens without inner loop reset.\n";

while(list(,$o) = each($arrayOuter)){
	while(list(,$i) = each($arrayInner)){
		print "inloop $i for $o\n";
	}
}
reset($arrayOuter);
reset($arrayInner);

print "What happens without inner loop reset but copy.\n";

while(list(,$o) = each($arrayOuter)){
	$placeholder = $arrayInner;
	while(list(,$i) = each($arrayInner)){
		print "inloop $i for $o\n";
	}
}
reset($arrayOuter);
reset($arrayInner);

print "What happens with inner loop reset over copy.\n";

while(list(,$o) = each($arrayOuter)){
	$placeholder = $arrayInner;
	while(list(,$i) = each($placeholder)){
		print "inloop $i for $o\n";
	}
}
reset($arrayOuter);
reset($arrayInner);
?>
--EXPECTF--
Correct - with inner loop reset.

Deprecated: The each() function is deprecated. This message will be suppressed on further calls in %s on line %d
inloop 0 for key1
inloop 1 for key1
inloop 0 for key2
inloop 1 for key2
What happens without inner loop reset.
inloop 0 for key1
inloop 1 for key1
What happens without inner loop reset but copy.
inloop 0 for key1
inloop 1 for key1
inloop 0 for key2
inloop 1 for key2
What happens with inner loop reset over copy.
inloop 0 for key1
inloop 1 for key1
inloop 0 for key2
inloop 1 for key2
