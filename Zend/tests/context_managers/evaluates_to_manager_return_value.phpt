--TEST--
Context manager: manager must not yield twice
--FILE--
<?php

function some_context() {
    yield 'hello';
    return 'world';
}

$result = using (some_context() => $var) {
    echo "Body executed: ";
    var_dump($var);
};

echo "Result: ";
var_dump($result);

?>
--EXPECT--
Body executed: string(5) "hello"
Result: string(5) "world"
