--TEST--
test_scheduler: exit() in the main flow does not raise the shutdown notification
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

/* The main flow leaves through the end-of-main handover, not through the
 * shutdown notification. */
echo "main: exiting\n";
exit(3);
?>
--EXPECT--
main: exiting
