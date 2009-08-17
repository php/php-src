--TEST--
Test uniqid() function : basic functionality
--FILE--
<?php
/* Prototype  : string uniqid  ([ string $prefix= ""  [, bool $more_entropy= false  ]] )
 * Description: Gets a prefixed unique identifier based on the current time in microseconds. 
 * Source code: ext/standard/uniqid.c
*/
echo "*** Testing uniqid() : basic functionality ***\n";

echo "\nuniqid() without a prefix\n";
var_dump(uniqid());
var_dump(uniqid(null, true));
var_dump(uniqid(null, false));
echo "\n\n";

echo "uniqid() with a prefix\n";

// Use a fixed prefix so we can ensure length of o/p id is fixed 
$prefix = array (
				99999,
				"99999",
				10.5e2,
				null,
				true,
				false				
				);

for ($i = 0; $i < count($prefix); $i++) {				
	var_dump(uniqid($prefix[$i]));
	var_dump(uniqid($prefix[$i], true));
	var_dump(uniqid($prefix[$i], false));
	echo "\n";
}	

?>
===DONE===
--EXPECTF-- 
*** Testing uniqid() : basic functionality ***

uniqid() without a prefix
unicode(13) "%s"
unicode(23) "%s.%s"
unicode(13) "%s"


uniqid() with a prefix
unicode(18) "99999%s"
unicode(28) "99999%s.%s"
unicode(18) "99999%s"

unicode(18) "999994%s"
unicode(28) "999994%s.%s"
unicode(18) "999994%s"

unicode(17) "1050%s"
unicode(27) "1050%s.%s"
unicode(17) "1050%s"

unicode(13) "%s"
unicode(23) "%s.%s"
unicode(13) "%s"

unicode(14) "1%s"
unicode(24) "1%s.%s"
unicode(14) "1%s"

unicode(13) "%s"
unicode(23) "%s.%s"
unicode(13) "%s"

===DONE===
	