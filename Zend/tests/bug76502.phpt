--TEST--
Bug #76502: Chain of mixed exceptions and errors does not serialize properly
--FILE--
<?php

$examples = [
    "Exception(Exception())" => new Exception("outer", 0,  new Exception("inner")),
    "Error(Error())"         => new Error("outer", 0, new Error("inner")),
    "Error(Exception())"     => new Error("outer", 0, new Exception("inner")),
    "Exception(Error())"     => new Exception("outer", 0, new Error("inner"))
];

foreach ($examples as $name => $example) {
    $processed = unserialize(serialize($example));
    $processedPrev = $processed->getPrevious();
    echo "---- $name ----\n";
    echo "before: ", get_class($example), ".previous == ",
        get_class($example->getPrevious()), "\n";
    echo "after : ", get_class($processed), ".previous == ",
        $processedPrev ? get_class($processedPrev) : "null", "\n";
}

?>
--EXPECT--
---- Exception(Exception()) ----
before: Exception.previous == Exception
after : Exception.previous == Exception
---- Error(Error()) ----
before: Error.previous == Error
after : Error.previous == Error
---- Error(Exception()) ----
before: Error.previous == Exception
after : Error.previous == Exception
---- Exception(Error()) ----
before: Exception.previous == Error
after : Exception.previous == Error
