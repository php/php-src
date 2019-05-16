--TEST--
include()ing files should not raise "too many open files" error
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Script ended normally]
prompt> 
--FILE--
<?php

for ($i = 0; $i < 25000; ++$i) {
    include __DIR__.'/empty.inc';
}
