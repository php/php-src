--TEST--
StackFrame getters
--FILE--
<?php

function frames() {
    return StackFrame::getTrace();
}
$frame = frames()[0];

var_dump([
    'file' => $frame->getFile(),
    'line' => $frame->getLine(),
    'function' => $frame->getFunction(),
    'class' => $frame->getClass(),
    'type' => $frame->getType(),
    'object' => $frame->getObject(),
    'args' => $frame->getArgs(),
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
