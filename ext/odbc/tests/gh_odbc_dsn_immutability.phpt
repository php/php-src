--TEST--
odbc_connect() must not mutate the caller's DSN string
--EXTENSIONS--
odbc
--FILE--
<?php
/* The '=' selects the connection-string form and the trailing ';' is what the
 * UID/PWD assembly used to overwrite with a NUL, in the caller's own buffer. */
$dsn = 'Driver=DoesNotExist;Database=x;SS001;';
const DSN_CONST = 'Driver=DoesNotExist;Database=x;SS001_CONST;';

@odbc_connect($dsn, 'u', 'p');
@odbc_connect(DSN_CONST, 'u', 'p');

var_dump($dsn);
var_dump(DSN_CONST);
?>
--EXPECT--
string(37) "Driver=DoesNotExist;Database=x;SS001;"
string(43) "Driver=DoesNotExist;Database=x;SS001_CONST;"
