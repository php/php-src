--TEST--
resource return type: non-resource
--FILE--
<?php

function bar(): resource {
    return true;
}

try {
	bar();
} catch (TypeError $e) {
	echo $e->getMessage();
}

--EXPECT--
Return value of bar() must be of the type resource, boolean returned

