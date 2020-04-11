--TEST--
Pipe operator fails void return chaining in strict mode
--FILE--
<?php
declare(strict_types=1);

function nonReturnFunction($bar): void {}

try {
    $result = "Hello World"
        |> 'nonReturnFunction'
        |> 'strlen';
    var_dump($result);
}
catch (Throwable $e) {
    printf("Expected %s thrown, got %s", TypeError::class, get_class($e));
}

?>
--EXPECT--
Expected TypeError thrown, got TypeError
