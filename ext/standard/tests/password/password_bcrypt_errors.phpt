--TEST--
Test error operation of password_hash() with bcrypt hashing
--FILE--
<?php
//-=-=-=-
try {
    password_hash("foo", PASSWORD_BCRYPT, array("cost" => 3));
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    var_dump(password_hash("foo", PASSWORD_BCRYPT, array("cost" => 32)));
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    var_dump(password_hash("null\0password", PASSWORD_BCRYPT));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: Invalid bcrypt cost parameter specified: 3
ValueError: Invalid bcrypt cost parameter specified: 32
ValueError: Bcrypt password must not contain null character
