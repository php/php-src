--TEST--
Null pipes in proc_open()
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');
$cmd = [$php, '-r', 'echo "Test"; fprintf(STDERR, "Error");'];

$proc = proc_open($cmd, [1 => ['null'], 2 => ['pipe', 'w']], $pipes);
var_dump($pipes);
var_dump(stream_get_contents($pipes[2]));
proc_close($proc);

$proc = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['null']], $pipes);
var_dump($pipes);
var_dump(stream_get_contents($pipes[1]));
proc_close($proc);

?>
--EXPECT--
array(1) {
  [2]=>
  resource(4) of type (stream)
}
string(5) "Error"
array(1) {
  [1]=>
  resource(6) of type (stream)
}
string(4) "Test"
