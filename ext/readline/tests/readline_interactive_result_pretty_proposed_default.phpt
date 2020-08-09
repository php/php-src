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
$write_line(<<<'EOT'
readline_interactive_shell_result_function(
    function(string $code, $result) {
        if (!isset($result)) {
            return;
        }
        if (is_scalar($result)) {
            echo "=> " . var_export($result, true) . "\n";
        } else {
            echo "=> "; var_dump($result);
        }});
EOT);
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
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
resource(5) of type (process)
Interactive shell

php > readline_interactive_shell_result_function(
php (     function(string $code, $result) {
php (         if (!isset($result)) {
php (             return;
php (         }
php (         if (is_scalar($result)) {
php (             echo "=> " . var_export($result, true) . "\n";
php (         } else {
php (             echo "=> "; var_dump($result);
php (         }});
php > 1+1;
=> 2
php > 0.5 * 2;
=> 1.0
php > namespace\MyClass::class;
=> 'MyClass'
php > fn()=>true;
=> object(Closure)#2 (0) {
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
=> object(stdClass)#2 (1) {
  ["key"]=>
  string(5) "value"
}
php > throw new RuntimeException("test");

Warning: Uncaught RuntimeException: test in php shell code:1
Stack trace:
#0 {main}
  thrown in php shell code on line 1
php >
