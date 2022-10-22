--TEST--
Bug GH-9801 (Crash when memory limit is exceeded during generator initialization)
--INI--
memory_limit=16m
--FILE--
<?php

function a() {
    yield from a();
}

foreach(a() as $v);
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s
