--TEST--
Bug #48693 (Double declaration of __lambda_func when lambda wrongly formatted)
--FILE--
<?php

try {
	$x = create_function('', 'return 1; }');
} catch (ParseError $e) {
	echo "$e\n\n";
}
try {
	$y = create_function('', 'function a() { }; return 2;');
} catch (ParseError $e) {
	echo "$e\n\n";
}
try {
	$z = create_function('', '{');
} catch (ParseError $e) {
	echo "$e\n\n";
}
try {
	$w = create_function('', 'return 3;');
} catch (ParseError $e) {
	echo "$e\n\n";
}

var_dump(
	$y(),
	$w()
);

?>
--EXPECTF--
Deprecated: Function create_function() is deprecated in %s on line %d
ParseError: syntax error, unexpected '}', expecting end of file in %sbug48693.php(4) : runtime-created function:1
Stack trace:
#0 %sbug48693.php(4): create_function('', 'return 1; }')
#1 {main}


Deprecated: Function create_function() is deprecated in %s on line %d

Deprecated: Function create_function() is deprecated in %s on line %d
ParseError: syntax error, unexpected end of file in %sbug48693.php(14) : runtime-created function:1
Stack trace:
#0 %sbug48693.php(14): create_function('', '{')
#1 {main}


Deprecated: Function create_function() is deprecated in %s on line %d
int(2)
int(3)
