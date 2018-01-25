--TEST--
Testing calls to anonymous function
--FILE--
<?php

for ($i = 0; $i < 10; $i++) {
	$a = create_function('', 'return '. $i .';');
	var_dump($a());

	$b = "\0lambda_". ($i + 1);
	var_dump($b());
}

?>
--EXPECTF--
Deprecated: Function create_function() is deprecated in %s on line %d
int(0)
int(0)

Deprecated: Function create_function() is deprecated in %s on line %d
int(1)
int(1)

Deprecated: Function create_function() is deprecated in %s on line %d
int(2)
int(2)

Deprecated: Function create_function() is deprecated in %s on line %d
int(3)
int(3)

Deprecated: Function create_function() is deprecated in %s on line %d
int(4)
int(4)

Deprecated: Function create_function() is deprecated in %s on line %d
int(5)
int(5)

Deprecated: Function create_function() is deprecated in %s on line %d
int(6)
int(6)

Deprecated: Function create_function() is deprecated in %s on line %d
int(7)
int(7)

Deprecated: Function create_function() is deprecated in %s on line %d
int(8)
int(8)

Deprecated: Function create_function() is deprecated in %s on line %d
int(9)
int(9)
