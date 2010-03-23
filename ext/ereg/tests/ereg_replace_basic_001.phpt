--TEST--
Test ereg_replace() function : basic functionality 
--FILE--
<?php
/* Prototype  : proto string ereg_replace(string pattern, string replacement, string string)
 * Description: Replace regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test a number of simple, valid matches with ereg_replace
 */

echo "*** Testing ereg() : basic functionality ***\n";

include(dirname(__FILE__) . '/regular_expressions.inc');

$replacement = '[this is a replacement]';

foreach ($expressions as $re) {
	list($pattern, $match) = $re;
	echo "--> Pattern: '$pattern'; match: '$match'\n";
	var_dump(ereg_replace($pattern, $replacement, $match . ' this contains some matches ' . $match));
}

echo "Done";
?>
--EXPECTF--
*** Testing ereg() : basic functionality ***
--> Pattern: '..(a|b|c)(a|b|c)..'; match: '--- ab ---'

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(82) "--[this is a replacement]-- this contains some matches --[this is a replacement]--"
--> Pattern: '()'; match: ''

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(695) "[this is a replacement] [this is a replacement]t[this is a replacement]h[this is a replacement]i[this is a replacement]s[this is a replacement] [this is a replacement]c[this is a replacement]o[this is a replacement]n[this is a replacement]t[this is a replacement]a[this is a replacement]i[this is a replacement]n[this is a replacement]s[this is a replacement] [this is a replacement]s[this is a replacement]o[this is a replacement]m[this is a replacement]e[this is a replacement] [this is a replacement]m[this is a replacement]a[this is a replacement]t[this is a replacement]c[this is a replacement]h[this is a replacement]e[this is a replacement]s[this is a replacement] [this is a replacement]"
--> Pattern: '()'; match: 'abcdef'

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(983) "[this is a replacement]a[this is a replacement]b[this is a replacement]c[this is a replacement]d[this is a replacement]e[this is a replacement]f[this is a replacement] [this is a replacement]t[this is a replacement]h[this is a replacement]i[this is a replacement]s[this is a replacement] [this is a replacement]c[this is a replacement]o[this is a replacement]n[this is a replacement]t[this is a replacement]a[this is a replacement]i[this is a replacement]n[this is a replacement]s[this is a replacement] [this is a replacement]s[this is a replacement]o[this is a replacement]m[this is a replacement]e[this is a replacement] [this is a replacement]m[this is a replacement]a[this is a replacement]t[this is a replacement]c[this is a replacement]h[this is a replacement]e[this is a replacement]s[this is a replacement] [this is a replacement]a[this is a replacement]b[this is a replacement]c[this is a replacement]d[this is a replacement]e[this is a replacement]f[this is a replacement]"
--> Pattern: '[x]|[^x]'; match: 'abcdef'

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(920) "[this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement]"
--> Pattern: '(a{1})(a{1,}) (b{1,3}) (c+) (d?ddd|e)'; match: '--- aaa bbb ccc ddd ---'

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(90) "--- [this is a replacement] --- this contains some matches --- [this is a replacement] ---"
--> Pattern: '\\\`\^\.\[\$\(\)\|\*\+\?\{\''; match: '\`^.[$()|*+?{''

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(74) "[this is a replacement] this contains some matches [this is a replacement]"
--> Pattern: '\a'; match: 'a'

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(118) "[this is a replacement] this cont[this is a replacement]ins some m[this is a replacement]tches [this is a replacement]"
--> Pattern: '[0-9][^0-9]'; match: '2a'

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(74) "[this is a replacement] this contains some matches [this is a replacement]"
--> Pattern: '^[[:alnum:]]{62,62}$'; match: '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(152) "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ this contains some matches 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
--> Pattern: '^[[:digit:]]{5}'; match: '0123456789'

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(66) "[this is a replacement]56789 this contains some matches 0123456789"
--> Pattern: '[[:digit:]]{5}$'; match: '0123456789'

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(66) "0123456789 this contains some matches 01234[this is a replacement]"
--> Pattern: '[[:blank:]]{1,10}'; match: '
 	'

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(163) "
[this is a replacement]this[this is a replacement]contains[this is a replacement]some[this is a replacement]matches[this is a replacement]
[this is a replacement]"
--> Pattern: '[[:print:]]{3}'; match: ' a '

Deprecated: Function ereg_replace() is deprecated in %s on line %d
string(254) "[this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement][this is a replacement] "
Done
