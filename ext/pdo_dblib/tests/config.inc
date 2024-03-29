<?php

// bug #72969 reflects a bug with FreeTDS, not with pdo_dblib
// this function will version detect so the relevant tests can XFAILIF
// assume this bug isn't present if not using FreeTDS
// otherwise require FreeTDS>=1.1
function driver_supports_batch_statements_without_select($db) {
    $version = $db->getAttribute(PDO::DBLIB_ATTR_VERSION);
    return !strstartswith($version, 'freetds ') || !strstartswith($version, 'freetds v1.0');
}

function strstartswith($haystack, $needle) {
    return $needle === "" || strrpos($haystack, $needle, -strlen($haystack)) !== false;
}

function getCredentials() {
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

function setAttributes(PDO $db) {
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);
}

function getDbConnection(string $class = PDO::class, ?array $attributes = null) {
    [$dsn, $user, $pass] = getCredentials();

    try {
        $db = new $class($dsn, $user, $pass, $attributes);
        if ($attributes === null) {
            setAttributes($db);
        }
    } catch (PDOException $e) {
        die('skip ' . $e->getMessage());
    }

    return $db;
}

function connectToDb() {
    [$dsn, $user, $pass] = getCredentials();

    try {
        $db = PDO::connect($dsn, $user, $pass);
        setAttributes($db);
    } catch (PDOException $e) {
        die('skip ' . $e->getMessage());
    }

    return $db;
}

?>
