<?php

function getDsn()
{
    if (false !== getenv('PDO_PGSQL_TEST_DSN')) {
        $dsn = getenv('PDO_PGSQL_TEST_DSN');
    } else {
        $dsn = 'pgsql:host=localhost port=5432 dbname=test user=postgres password=postgres';
    }

    return $dsn;
//    if (false !== getenv('PDO_PGSQL_TEST_USER')) {
//        $user = getenv('PDO_PGSQL_TEST_USER');
//    } else {
//        $user = 'postgres';
//    }
//
//    if (false !== getenv('PDO_PGSQL_TEST_PASS')) {
//        $pass = getenv('PDO_PGSQL_TEST_PASS');
//    } else {
//        $pass = 'postgres';
//    }
//
//    return [$dsn, $user, $pass];
}
