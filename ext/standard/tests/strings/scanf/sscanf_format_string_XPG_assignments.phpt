--TEST--
sscanf(): format string XPG argument tests
--FILE--
<?php


$formats = [
	'%1$s %2$s %1$s',
	'%1$s %*s %3$s',
	'%s %*s %3$s',
];

$str = "Hello World";

foreach ($formats as $format) {
	echo "Using format string '$format':\n";
	try {
	    var_dump(sscanf($str, $format));
	} catch (Throwable $e) {
	    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
	    var_dump(sscanf($str, $format, $a, $b, $c));
	    var_dump($a, $b, $c);
	    $a = $b = $c = null;
	} catch (Throwable $e) {
	    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}
?>
--EXPECT--
Using format string '%1$s %2$s %1$s':
ValueError: sscanf(): Argument #2 ($format) argument 1 is assigned by multiple "%n$" conversion specifiers
ValueError: sscanf(): Argument #3 is assigned by multiple "%n$" conversion specifiers
Using format string '%1$s %*s %3$s':
array(3) {
  [0]=>
  string(5) "Hello"
  [1]=>
  NULL
  [2]=>
  NULL
}
ValueError: sscanf(): Argument #4 is not assigned by any conversion specifiers
Using format string '%s %*s %3$s':
ValueError: sscanf(): Argument #2 ($format) cannot mix "%" and "%n$" conversion specifiers
ValueError: sscanf(): Argument #2 ($format) cannot mix "%" and "%n$" conversion specifiers
