--TEST--
register_tick_function() & closure
--FILE--
<?php

declare (ticks = 1);

$i = 0;
register_tick_function(function () use (&$i) { $i++; });

echo "Test\n";
echo "$i\n";
echo "$i\n";
var_dump ($i != 0);
echo "$i\n";
echo "Done\n";

?>
--EXPECTF--	
Test
%d
%d
bool(true)
%d
Done
