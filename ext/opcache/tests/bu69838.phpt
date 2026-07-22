--TEST--
Bug #69838 OPCACHE Warning Internal error: wrong size calculation
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.log_verbosity_level=2
opcache.interned_strings_buffer=8
--EXTENSIONS--
opcache
--FILE--
<?php

$x = gethostbyname("localhost");

?>
===DONE===
--EXPECTF--
%r(?:.*: Warning Apple Silicon ZTS JIT requires pthread_jit_write_protect_np\(\) support\. JIT disabled\.\n)?%r===DONE===
