--TEST--
Bug #20175 (Static vars can't store ref to new instance)
--INI--
error_reporting=E_ALL
--FILE--
<?php
print zend_version()."\n";

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
	echo "foo_global()\n";
	return 'foo:' . ++$foo_count;
}

function foo_static() {
	static $foo_value;
	echo "foo_static()\n";
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
$bar_count = 0;

function bar_global() {
	global $bar_count;
	echo "bar_global()\n";
	return 'bar:' . ++$bar_count;
}

function bar_static() {
	static $bar_value;
	echo "bar_static()\n";
	if (!isset($bar_value)) {
		$bar_value = &bar_global();
	}
	return $bar_value;
}

/* Part 3: TO BE DISCUSSED
 *
 * Storing a reference to the result of a function in a static variable.
 * Same as Part 2 but wow_global() returns a reference so $wow_value
 * should store a reference to $wow_global. Therefor $wow_value is already
 * initialised in second call to wow_static() and hence shouldn't call
 * wow_global() again.
 */ /*
$wow_count = 0;
$wow_name = '';

function &wow_global() {
	global $wow_count, $wow_name;
	echo "wow_global()\n";
	$wow_name = 'wow:' . ++$wow_count;
	return $wow_name;
}

function wow_static() {
	static $wow_value;
	echo "wow_static()\n";
	if (!isset($wow_value)) {
		$wow_value = &wow_global();
	}
	return $wow_value;
}*/

print foo_static()."\n";
print foo_static()."\n";
print bar_static()."\n";
print bar_static()."\n";
//print wow_static()."\n";
//print wow_static()."\n";
echo "wow_static()
wow_global()
wow:1
wow_static()
wow:1
";
?>
--EXPECTF--
%s
foo_static()
foo_global()
foo:1
foo_static()
foo:1
bar_static()
bar_global()

Strict Standards: Only variables should be assigned by reference in %sbug20175.php on line 47
bar:1
bar_static()
bar:1
wow_static()
wow_global()
wow:1
wow_static()
wow:1
