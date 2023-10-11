--TEST--
Stdin and escaped args being passed to run command
--CLEAN--
<?php
@unlink("run_002_tmp.fixture");
?>
--PHPDBG--
ev file_put_contents("run_002_tmp.fixture", "stdin\ndata")
b 6
r <run_002_tmp.fixture
r arg1 '_ \' arg2 "' < run_002_tmp.fixture
y
c
q
--EXPECTF--
[Successful compilation of %s]
prompt> 10
prompt> [Breakpoint #0 added at %s:6]
prompt> array(1) {
  [0]=>
  string(%d) "%s"
}
string(10) "stdin
data"
[Breakpoint #0 at %s:6, hits: 1]
>00006: echo "ok\n";
 00007: 
prompt> Do you really want to restart execution? (type y or n): array(3) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(4) "arg1"
  [2]=>
  string(10) "_ ' arg2 ""
}
string(10) "stdin
data"
[Breakpoint #0 at %s:6, hits: 1]
>00006: echo "ok\n";
 00007: 
prompt> ok
[Script ended normally]
prompt> 
--FILE--
<?php

var_dump($argv);
var_dump(stream_get_contents(STDIN));

echo "ok\n";
?>
