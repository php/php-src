<?php

require_once dirname(__DIR__, 3) . '/tests/probe_cache.inc';

// bug #72969 reflects a bug with FreeTDS, not with pdo_dblib
// this function will version detect so the relevant tests can XFAILIF
// assume this bug isn't present if not using FreeTDS
// otherwise require FreeTDS>=1.1
function driver_supports_batch_statements_without_select($db) {
    $version = $db->getAttribute(Pdo\Dblib::ATTR_VERSION);
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
    $connectionAttributes = $attributes;
    $evaluatingSkipif = getenv('TEST_PHP_EVALUATING_SKIPIF') === '1';
    if ($evaluatingSkipif && $connectionAttributes === null && getenv('PDO_DBLIB_TEST_DSN') === false) {
        $connectionAttributes = [Pdo\Dblib::ATTR_CONNECTION_TIMEOUT => 1];
    }
    [$dsn, $user, $pass] = getCredentials();

    $db = null;
    $probe = static function () use (&$db, $class, $dsn, $user, $pass, $attributes, $connectionAttributes): ?string {
        try {
            $db = new $class($dsn, $user, $pass, $connectionAttributes);
            if ($attributes === null) {
                setAttributes($db);
            }
            return null;
        } catch (PDOException $e) {
            return $e->getMessage();
        }
    };

    $reason = $evaluatingSkipif
        ? ProbeCache::getFailure('pdo', [$class, false, $dsn, $user, $pass, $connectionAttributes], $probe)
        : $probe();

    if (is_string($reason)) {
        die('skip ' . $reason);
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
