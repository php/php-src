--TEST--
include_once must include only once #2
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> 1
[Script ended normally]
prompt> 
--FILE--
<?php

include __DIR__.'/include.inc';
include_once __DIR__.'/include.inc';
