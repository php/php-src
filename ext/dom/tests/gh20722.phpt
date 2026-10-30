--TEST--
GH-20722 (Null pointer dereference in DOM namespace node cloning via clone on malformed objects)
--EXTENSIONS--
dom
--FILE--
<?php

clone new DOMNameSpaceNode();
echo "Done";

?>
--EXPECT--
Done
