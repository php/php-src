--TEST--
Testing register_shutdown_function() with timeout. (Bug: #21513)
--FILE--
<?php

ini_set('display_errors', 0);
    
echo "Start\n";

function boo()
{
	echo "Shutdown\n";
}

register_shutdown_function("boo");

/* not necessary, just to show the error sooner */
set_time_limit(1); 

/* infinite loop to simulate long processing */
for (;;) {}

echo "End\n";

?>
--EXPECT--
Start
Shutdown
