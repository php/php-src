--TEST--
StackFrame read dimensions
--FILE--
<?php

function frames() {
    return StackFrame::getTrace();
}
$frame = frames()[0];

var_dump([
    'file' => $frame['file'],
    'line' => $frame['line'],
    'function' => $frame['function'],
    'class' => $frame['class'],
    'type' => $frame['type'],
    'object' => $frame['object'],
    'args' => $frame['args'],
]);

--EXPECTF--
array(7) {
  ["file"]=>
  string(%d) "%s.php"
  ["line"]=>
  int(%d)
  ["function"]=>
  string(6) "frames"
  ["class"]=>
  NULL
  ["type"]=>
  NULL
  ["object"]=>
  NULL
  ["args"]=>
  array(0) {
  }
}
