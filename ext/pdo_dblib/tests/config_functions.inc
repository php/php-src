<?php

function getDsnUserAndPassword()
{

    if (false !== getenv('PDO_DBLIB_TEST_DSN')) {
        $dsn = getenv('PDO_DBLIB_TEST_DSN');
    } else {
        $dsn = 'dblib:host=localhost;dbname=test';
    }

    if (false !== getenv('PDO_DBLIB_TEST_USER')) {
        $user = getenv('PDO_DBLIB_TEST_USER');
    } else {
        $user = 'php';
    }

    if (false !== getenv('PDO_DBLIB_TEST_PASS')) {
        $pass = getenv('PDO_DBLIB_TEST_PASS');
    } else {
        $pass = 'password';
    }
    return [$dsn, $user, $pass];
}
