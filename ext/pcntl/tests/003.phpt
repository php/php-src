--TEST--
pcntl: SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!function_exists("pcntl_sigprocmask")) die("skip pcntl_sigprocmask() not available");
?>
--FILE--
<?php

const SIGNO_NAMES = [
    SIGINT => "SIGINT",
    SIGTERM => "SIGTERM",
    SIGCHLD => "SIGCHLD",
];

function map_signo_to_name(int $no): string {
    return SIGNO_NAMES[$no];
}

// Clear mask
pcntl_sigprocmask(SIG_SETMASK, [], $prev);

pcntl_sigprocmask(SIG_BLOCK, [SIGCHLD, SIGTERM], $old);
var_dump(array_map(map_signo_to_name(...), $old));
pcntl_sigprocmask(SIG_BLOCK, [SIGINT], $old);
var_dump(array_map(map_signo_to_name(...), $old));
pcntl_sigprocmask(SIG_UNBLOCK, [SIGINT], $old);
var_dump(array_map(map_signo_to_name(...), $old));
pcntl_sigprocmask(SIG_SETMASK, [SIGINT], $old);
var_dump(array_map(map_signo_to_name(...), $old));
pcntl_sigprocmask(SIG_SETMASK, [], $old);
var_dump(array_map(map_signo_to_name(...), $old));

// Restore previous mask
pcntl_sigprocmask(SIG_SETMASK, $prev, $old);
var_dump(array_map(map_signo_to_name(...), $old));

?>
--EXPECT--
array(0) {
}
array(2) {
  [0]=>
  string(7) "SIGTERM"
  [1]=>
  string(7) "SIGCHLD"
}
array(3) {
  [0]=>
  string(6) "SIGINT"
  [1]=>
  string(7) "SIGTERM"
  [2]=>
  string(7) "SIGCHLD"
}
array(2) {
  [0]=>
  string(7) "SIGTERM"
  [1]=>
  string(7) "SIGCHLD"
}
array(1) {
  [0]=>
  string(6) "SIGINT"
}
array(0) {
}
