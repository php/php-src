--TEST--
Pipe operator throws normally on missing function
--FILE--
<?php

try {
    $res1 = 5 |> '_test';
}
catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Call to undefined function _test()
