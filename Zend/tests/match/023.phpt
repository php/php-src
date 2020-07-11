--TEST--
Test match strict comparison with true expression
--FILE--
<?php

function wrong() {
    throw new Exception();
}

echo match (true) {
    'truthy' => wrong(),
    ['truthy'] => wrong(),
    new stdClass() => wrong(),
    1 => wrong(),
    1.0 => wrong(),
    true => "true\n",
};

?>
--EXPECT--
true
