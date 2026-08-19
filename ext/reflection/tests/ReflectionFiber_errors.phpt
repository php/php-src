--TEST--
ReflectionFiber errors
--FILE--
<?php

$fiber = new Fiber(function (): void {
    Fiber::suspend();
});

$reflection = new ReflectionFiber($fiber);

try {
    $reflection->getTrace();
} catch (Throwable $error) {
    echo $error::class, ': ', $error->getMessage(), "\n";
}

try {
    $reflection->getExecutingFile();
} catch (Throwable $error) {
    echo $error::class, ': ', $error->getMessage(), "\n";
}

try {
    $reflection->getExecutingLine();
} catch (Throwable $error) {
    echo $error::class, ': ', $error->getMessage(), "\n";
}

$fiber->start();

var_dump($reflection->getExecutingFile());
var_dump($reflection->getExecutingLine());

$fiber->resume();

try {
    $reflection->getTrace();
} catch (Throwable $error) {
    echo $error::class, ': ', $error->getMessage(), "\n";
}

try {
    $reflection->getExecutingFile();
} catch (Throwable $error) {
    echo $error::class, ': ', $error->getMessage(), "\n";
}

try {
    $reflection->getExecutingLine();
} catch (Throwable $error) {
    echo $error::class, ': ', $error->getMessage(), "\n";
}

try {
    $reflection->getCallable();
} catch (Throwable $error) {
    echo $error::class, ': ', $error->getMessage(), "\n";
}

?>
--EXPECTF--
Error: Cannot fetch information from a fiber that has not been started or is terminated
Error: Cannot fetch information from a fiber that has not been started or is terminated
Error: Cannot fetch information from a fiber that has not been started or is terminated
string(%d) "%s%eReflectionFiber_errors.php"
int(4)
Error: Cannot fetch information from a fiber that has not been started or is terminated
Error: Cannot fetch information from a fiber that has not been started or is terminated
Error: Cannot fetch information from a fiber that has not been started or is terminated
Error: Cannot fetch the callable from a fiber that has terminated
