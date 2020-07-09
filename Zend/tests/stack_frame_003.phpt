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
    'object_class' => $frame->getObjectClass(),
    'closure' => $frame->getClosure(),
]);

--EXPECTF--
array(%d) {
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
  ["object_class"]=>
  NULL
  ["closure"]=>
  NULL
}
