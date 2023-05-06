<?php

function getDsnUserAndPassword()
{
    if (false !== getenv('PDO_MYSQL_TEST_DSN')) {
        $dsn = getenv('PDO_MYSQL_TEST_DSN');
    } else {
        die('skip - PDO_MYSQL_TEST_DSN must be set');
    }

    if (false !== getenv('PDO_MYSQL_TEST_USER')) {
        $user = getenv('PDO_MYSQL_TEST_USER');
    } else {
        $user = null;
    }

    if (false !== getenv('PDO_MYSQL_TEST_PASS')) {
        $pass = getenv('PDO_MYSQL_TEST_PASS');
    } else {
        $pass = null;
    }

    return [$dsn, $user, $pass];
}
