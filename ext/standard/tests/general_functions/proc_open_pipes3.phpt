--TEST--
proc_open() with invalid pipes
--FILE--
<?php

for ($i = 3; $i<= 5; $i++) {
    $spec[$i] = array('pipe', 'w');
}

$php = getenv("TEST_PHP_EXECUTABLE");
$callee = __DIR__ . "/proc_open_pipes_sleep.inc";

$spec[$i] = array('pi');
proc_open("$php -n $callee", $spec, $pipes);

$spec[$i] = 1;
try {
    proc_open("$php -n $callee", $spec, $pipes);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

$spec[$i] = array('pipe', "test");
proc_open("$php -n $callee", $spec, $pipes);
var_dump($pipes);

$spec[$i] = array('file', "test", "z");
proc_open("$php -n $callee", $spec, $pipes);
var_dump($pipes);

echo "END\n";
?>
--EXPECTF--
Warning: proc_open(): pi is not a valid descriptor spec/mode in %s on line %d
proc_open(): Argument #2 ($descriptor_spec) must only contain arrays and streams
array(4) {
  [3]=>
  resource(%d) of type (Unknown)
  [4]=>
  resource(%d) of type (Unknown)
  [5]=>
  resource(%d) of type (Unknown)
  [6]=>
  resource(%d) of type (Unknown)
}

Warning: proc_open(test): Failed to open stream: %s in %s on line %d
array(4) {
  [3]=>
  resource(%d) of type (Unknown)
  [4]=>
  resource(%d) of type (Unknown)
  [5]=>
  resource(%d) of type (Unknown)
  [6]=>
  resource(%d) of type (Unknown)
}
END
