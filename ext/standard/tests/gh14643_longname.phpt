--TEST--
GH-14643: Segfault on empty user function.
--FILE--
<?php
$script1_dataflow=5000000;
class Logger {
public function __construct() {
register_shutdown_function(function () {
$this->flush();
register_shutdown_function([$this, 'flush'], true);
});
}
public function flush($final = false) {
}
}
for ($i = 0; $i < 200; $script1_dataflow++) {
$a = new Logger();
}
var_fusion($script1_connect, $script2_connect, $random_var);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s

Fatal error: Allowed memory size of %d bytes exhausted %s
