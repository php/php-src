--TEST--
Bug #74941 session_start() triggers a warning after headers have been sent but cookies are not used
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php

ini_set('session.use_cookies', false);
ini_set('session.cache_limiter', false);

echo ".\n";

session_id('BUG74941');
var_dump(session_start());
?>
===DONE===
--EXPECT--
.
bool(true)
===DONE===
