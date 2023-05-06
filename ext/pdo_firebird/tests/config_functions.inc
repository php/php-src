<?php

function getDsnUserAndPassword()
{
    if (false !== getenv('PDO_FIREBIRD_TEST_DSN')) {
        $dsn = getenv('PDO_FIREBIRD_TEST_DSN');
    } else {
        $dsn = 'firebird:dbname=/opt/firebird/test.gdb';
    }

    if (false !== getenv('PDO_FIREBIRD_TEST_USER')) {
        $user = getenv('PDO_FIREBIRD_TEST_USER');
    } else {
        $user = 'SYSDBA';
    }

    if (false !== getenv('PDO_FIREBIRD_TEST_PASS')) {
        $pass = getenv('PDO_FIREBIRD_TEST_PASS');
    } else {
        $pass = 'password';
    }
    return [$dsn, $user, $pass];
}
