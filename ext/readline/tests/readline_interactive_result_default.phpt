--TEST--
Configurable interactive results in readline
--INI--
cli.enable_interactive_shell_result_function=1
--SKIPIF--
<?php
if (!extension_loaded('readline')) die('skip readline extension not available');
if (READLINE_LIB !== 'readline') { die('skip readline only'); }
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -a", $descriptorspec, $pipes);
var_dump($proc);

// var_dump, json_encode, or any more complex dumping can be used in readline_interactive_shell_result_function.
$write_line = fn(string $line) => fwrite($pipes[0], $line . "\n");
$write_line('1+1;');
$write_line('0.5 * 2;');
$write_line('namespace\MyClass::class;');
$write_line('fn()=>true;');
$write_line('$x = ["foo", "bar"];');
$write_line('asort($x);');
$write_line('$x;');
$write_line('json_encode($x);');
$write_line('unset($x);');
$write_line('function do_something() { echo "in do_something()\n"; }');
$write_line('do_something();');
$write_line('json_decode(\'{"key": "value"}\');');
$write_line('throw new RuntimeException("test");');
$write_line('printf("newline is automatically appended by shell");');
$write_line('printf("newline not automatically appended by shell\n");');
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
resource(5) of type (process)
Interactive shell

php > 1+1;
=> 2
php > 0.5 * 2;
=> 1.0
php > namespace\MyClass::class;
=> 'MyClass'
php > fn()=>true;
=> object(Closure)#1 (0) {
}
php > $x = ["foo", "bar"];
=> array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
php > asort($x);
=> true
php > $x;
=> array(2) {
  [1]=>
  string(3) "bar"
  [0]=>
  string(3) "foo"
}
php > json_encode($x);
=> '{"1":"bar","0":"foo"}'
php > unset($x);
php > function do_something() { echo "in do_something()\n"; }
php > do_something();
in do_something()
php > json_decode('{"key": "value"}');
=> object(stdClass)#1 (1) {
  ["key"]=>
  string(5) "value"
}
php > throw new RuntimeException("test");

Warning: Uncaught RuntimeException: test in php shell code:1
Stack trace:
#0 {main}
  thrown in php shell code on line 1
php > printf("newline is automatically appended by shell");
newline is automatically appended by shell
=> 42
php > printf("newline not automatically appended by shell\n");
newline not automatically appended by shell
=> 44
php >
