--TEST--
Context manager: VAR is optional
--FILE--
<?php

function some_context() {
    yield new stdClass;
}

using (some_context()) {
    echo "Body is executed once\n";
};

?>
--EXPECT--
Body is executed once
