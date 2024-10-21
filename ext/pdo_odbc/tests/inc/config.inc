<?php
$env = [
    'PDO_ODBC_TEST_DSN' => false !== getenv('PDO_ODBC_TEST_DSN') ? getenv('PDO_ODBC_TEST_DSN') : '',
    'PDO_ODBC_TEST_USER' => false !== getenv('PDO_ODBC_TEST_USER') ? getenv('PDO_ODBC_TEST_USER') : null,
    'PDO_ODBC_TEST_PASS' => false !== getenv('PDO_ODBC_TEST_PASS') ? getenv('PDO_ODBC_TEST_PASS') : null,
    'PDO_ODBC_TEST_ATTR' => false !== getenv('PDO_ODBC_TEST_ATTR') ? getenv('PDO_ODBC_TEST_ATTR') : null,
];

if (!$env['PDO_ODBC_TEST_DSN'] && preg_match('/^WIN/i', PHP_OS) && extension_loaded('com_dotnet')) {
    // on Windows and user didn't set PDOTEST_DSN, try this as a fallback:
    // check if MS Access DB is installed, and if yes, try using it. create a temporary MS access database.

    $path = realpath(__DIR__) . '\..\pdo_odbc.mdb';
    if (!file_exists($path)) {
        try {
            // try to create database
            $adox = new COM('ADOX.Catalog');
            $adox->Create('Provider=Microsoft.Jet.OLEDB.4.0;Data Source=' . $path);
            $adox = null;

        } catch (Exception $e) {
            // do nothing
        }
    }
    if (file_exists($path)) {
        // database was created and written to file system
        $env['PDO_ODBC_TEST_DSN'] = 'odbc:Driver={Microsoft Access Driver (*.mdb)};Dbq=$path;Uid=Admin';
    }
}
?>
