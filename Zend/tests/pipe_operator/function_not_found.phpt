--TEST--
Pipe operator throws normally on missing function
--FILE--
<?php

try {
    $res1 = 5 |> '_test';
}
catch (Throwable $e) {
    printf("Expected %s thrown, got %s", Error::class, get_class($e));
}

?>
--EXPECT--
Expected Error thrown, got Error
