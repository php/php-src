--TEST--
PostgreSQL fetching default link automatically is deprecated
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');
$db = pg_connect($conn_str);

// We don't care about warnings
ini_set('error_reporting', E_ALL&~E_WARNING);

// Special cases
pg_set_error_verbosity(PGSQL_ERRORS_TERSE);
pg_query_params('str', []);
pg_execute('str', []);

pg_lo_unlink(1);
// deprecated alias 'pg_lounlink',
pg_lo_open(1, 'r');
// deprecated alias 'pg_loopen',
pg_lo_export(1, '');
// deprecated alias 'pg_loexport',

$functions = [
'pg_dbname',
'pg_last_error',
// deprecated alias 'pg_errormessage',
'pg_options',
'pg_port',
'pg_tty',
'pg_host',
'pg_version',
'pg_parameter_status',
'pg_ping',
'pg_query',
'pg_exec',
'pg_prepare',
'pg_untrace',
'pg_lo_create',
// deprecated alias 'pg_locreate',
'pg_lo_import',
// deprecated alias 'pg_loimport',
'pg_set_client_encoding',
// deprecated alias 'pg_setclientencoding',
'pg_client_encoding',
// deprecated alias 'pg_clientencoding',
'pg_end_copy',
'pg_put_line',
'pg_escape_string',
'pg_escape_bytea',
'pg_escape_literal',
'pg_escape_identifier',
'pg_close',
];

foreach ($functions as $function) {
    $args = [];
    $argNum = 0;
    retry:
    try {
        $function(...$args);
        continue;
    } catch (ArgumentCountError) {
        $args[] = 'str';
        ++$argNum;
        goto retry;
    } catch (TypeError $e) {
        if (str_contains($e->getMessage(), 'type array')) {
            $args[$argNum] = [];
            goto retry;
        }
        var_dump($e->getMessage());
    } catch (\Error $e) {
        echo $e->getMessage();
    }
}

echo "END";
?>
--EXPECTF--
Deprecated: pg_set_error_verbosity(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_query_params(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_execute(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_lo_unlink(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_lo_open(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_lo_export(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_dbname(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_last_error(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_options(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_port(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_tty(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_host(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_version(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_parameter_status(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_ping(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_query(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_exec(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_prepare(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_untrace(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_lo_create(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_lo_import(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_set_client_encoding(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_client_encoding(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_end_copy(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_put_line(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_escape_string(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_escape_bytea(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_escape_literal(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_escape_identifier(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d

Deprecated: pg_close(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
END
