--TEST--
Bug #20175 (Static vars can't store ref to new instance)
--FILE--
<?php
/* Part 1:
 * Storing the result of a function in a static variable.
 * foo_global() increments global variable $foo_count whenever it is executed.
 * When foo_static() is called it checks for the static variable $foo_value
 * being initialised. In case initialisation is necessary foo_global() will be
 * called. Since that must happen only once the return value should be equal.
 */
$foo_count = 0;

function foo_global() {
	global $foo_count;
	return 'foo:' . ++$foo_count;
}

function foo_static() {
	static $foo_value;
	if (!isset($foo_value)) {
		$foo_value = foo_global();
	}
	return $foo_value;
}

/* Part 2:
 * Storing a reference to the result of a function in a static variable.
 * Same as Part 1 but:
 * The return statment transports a copy of the value to return. In other 
 * words the return value of bar_global() is a temporary variable only valid
 * after the function call bar_global() is done in current local scope.
 */
$bar_global = 0;

function bar_global() {
	global $bar_count;
	return 'bar:' . ++$bar_count;
}

function bar_static() {
	static $bar_value;
	if (!isset($bar_value)) {
		$bar_value = &bar_global();
	}
	return $bar_value;
}

/* Part 3:
 * Storing a reference to the result of a function in a static variable.
 * Same as Part 2 but wow_global() returns a reference so $wow_value
 * should store a reference to $wow_global. Therefor $wow_value is already
 * initialised in second call to wow_static() and hence shouldn't call
 * wow_global() again.
 */
$wow_global = 0;
$wow_name = '';

function &wow_global() {
	global $wow_count, $wow_name;
	$wow_name = 'wow:' . ++$wow_count;
	return $wow_name;
}

function wow_static() {
	static $wow_value;
	if (!isset($wow_value)) {
		$wow_value = &wow_global();
	}
	return $wow_value;
}

print zend_version()."\n";
print foo_static()."\n";
print foo_static()."\n";
print bar_static()."\n";
print bar_static()."\n";
print wow_static()."\n";
print wow_static()."\n";
?>
--EXPECTF--
%s
foo:1
foo:1
bar:1
bar:2
wow:1
wow:1
