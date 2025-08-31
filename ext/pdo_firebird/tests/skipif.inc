<?php
if (!getenv('PDO_FIREBIRD_TEST_DSN')) {
    die('skip: PDO_FIREBIRD_TEST_DSN must be set');
}
