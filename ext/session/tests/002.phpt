--TEST--
session_unset() without any session
--FILE--
<?php
session_unset();
print "ok\n";
--GET--
--POST--
--EXPECT--
ok
