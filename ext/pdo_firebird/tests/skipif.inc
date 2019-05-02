<?php

if(!extension_loaded('pdo_firebird'))
{
        die('skip: pdo_firebird not available');
}
elseif(!getenv('PDO_FIREBIRD_TEST_DATABASE'))
{
        die('skip: PDO_FIREBIRD_TEST_DATABASE must be set');
}

?>