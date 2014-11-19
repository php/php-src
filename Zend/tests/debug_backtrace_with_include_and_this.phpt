--TEST--
debug_backtrace segmentation fault with include and error handler 
--FILE--
<?php
class CLWrapper {
  function stream_open($path, $mode, $options, $opened_path) {
    return false;
  }
}

class CL {
  public function load($class) {
    if (!include($class)) {
      throw new Exception('Failed loading '.$class);
    }
  }
}

stream_wrapper_register('class', 'CLWrapper');
set_error_handler(function($code, $msg, $file, $line) {
  $bt= debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 2);
  echo "ERR#$code: $msg @ ", $bt[1]['function'], "\n";
});

try {
  (new CL())->load('class://non.existent.Class');
} catch (CLException $e) {
  echo $e."\n";
}
--EXPECTF--
ERR#2: include(class://non.existent.Class): failed to open stream: "CLWrapper::stream_open" call failed @ include
ERR#2: include(): Failed opening 'class://non.existent.Class' for inclusion (include_path='%s') @ include

Fatal error: Uncaught exception 'Exception' with message 'Failed loading class://non.existent.Class' in %s
Stack trace:
#0 %s(%d): CL->load('class://non.exi...')
#1 {main}
  thrown in %s on line %d

