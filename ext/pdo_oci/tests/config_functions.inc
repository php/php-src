<?php

function getDsnUserAndPassword()
{
    if (false !== getenv('PDO_OCI_TEST_DSN')) {
        $dsn = getenv('PDO_OCI_TEST_DSN');
    } else {
        die('skip - PDO_OCI_TEST_DSN must be set');
    }

    if (false !== getenv('PDO_OCI_TEST_USER')) {
        $user = getenv('PDO_OCI_TEST_USER');
    } else {
        $user = 'php';
    }

    if (false !== getenv('PDO_OCI_TEST_PASS')) {
        $pass = getenv('PDO_OCI_TEST_PASS');
    } else {
        $pass = 'password';
    }
    return [$dsn, $user, $pass];
}
