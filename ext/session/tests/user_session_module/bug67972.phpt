--TEST--
Bug #67972: SessionHandler Invalid memory read create_sid()
--EXTENSIONS--
session
--FILE--
<?php

try {
    (new SessionHandler)->create_sid();
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
Error: Session is not active
