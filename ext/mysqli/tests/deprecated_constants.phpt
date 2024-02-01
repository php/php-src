--TEST--
Deprecated constants
--EXTENSIONS--
mysqli
--FILE--
<?php

echo constant('MYSQLI_NO_DATA')."\n";
echo constant('MYSQLI_DATA_TRUNCATED')."\n";
echo constant('MYSQLI_SERVER_QUERY_NO_GOOD_INDEX_USED')."\n";
echo constant('MYSQLI_SERVER_QUERY_NO_INDEX_USED')."\n";
if (stristr(mysqli_get_client_info(), 'mysqlnd')) {
    echo constant('MYSQLI_SERVER_QUERY_WAS_SLOW')."\n";
    echo constant('MYSQLI_SERVER_PS_OUT_PARAMS')."\n";
} else {
    print("\nDeprecated: Constant MYSQLI_SERVER_QUERY_WAS_SLOW is deprecated in dummy\n-1\n");
    print("\nDeprecated: Constant MYSQLI_SERVER_PS_OUT_PARAMS is deprecated in dummy\n-1\n");
}
echo constant('MYSQLI_IS_MARIADB')."\n";

?>
--EXPECTF--

Deprecated: Constant MYSQLI_NO_DATA is deprecated in %s
%i

Deprecated: Constant MYSQLI_DATA_TRUNCATED is deprecated in %s
%i

Deprecated: Constant MYSQLI_SERVER_QUERY_NO_GOOD_INDEX_USED is deprecated in %s
%i

Deprecated: Constant MYSQLI_SERVER_QUERY_NO_INDEX_USED is deprecated in %s
%i

Deprecated: Constant MYSQLI_SERVER_QUERY_WAS_SLOW is deprecated in %s
%i

Deprecated: Constant MYSQLI_SERVER_PS_OUT_PARAMS is deprecated in %s
%i

Deprecated: Constant MYSQLI_IS_MARIADB is deprecated in %s
