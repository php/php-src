<?php

$rpc = new com("class");

/* class hirarchy test */
echo "is class .. " . (get_class($rpc) == "class" ? "passed" : "faiure");
echo "\n";
echo "is com   .. " . (is_subclass_of($rpc, "com") ? "passed" : "failure");
echo "\n";
echo "is rpc   .. " . (is_subclass_of($rpc, "rpc") ? "passed" : "failure");

/* uncloneable */
//$rpc->__clone(); // issues a fatal
?>