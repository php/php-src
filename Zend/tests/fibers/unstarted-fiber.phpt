--TEST--
Not starting a fiber does not leak
--FILE--
<?php

$fiber = new Fiber(fn() => null);

echo "done";

?>
--EXPECT--
done
