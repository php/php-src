--TEST--
Constants exported by ext/mysqli
--EXTENSIONS--
mysqli
--INI--
mysqli.allow_local_infile=1
--FILE--
<?php

$constants = get_defined_constants(true);
sort($constants);

$expected_constants = array(
    'MYSQLI_READ_DEFAULT_GROUP'			=> true,
    'MYSQLI_READ_DEFAULT_FILE'			=> true,
    'MYSQLI_OPT_CONNECT_TIMEOUT'		=> true,
    'MYSQLI_OPT_LOCAL_INFILE'			=> true,
    'MYSQLI_OPT_READ_TIMEOUT'			=> true,
    'MYSQLI_INIT_COMMAND'				=> true,
    'MYSQLI_CLIENT_SSL'					=> true,
    "MYSQLI_CLIENT_COMPRESS"			=> true,
    "MYSQLI_CLIENT_INTERACTIVE"			=> true,
    "MYSQLI_CLIENT_IGNORE_SPACE"		=> true,
    "MYSQLI_CLIENT_NO_SCHEMA"			=> true,
    "MYSQLI_CLIENT_FOUND_ROWS"			=> true,
    "MYSQLI_STORE_RESULT"				=> true,
    "MYSQLI_USE_RESULT"					=> true,
    "MYSQLI_ASSOC"						=> true,
    "MYSQLI_NUM"						=> true,
    "MYSQLI_BOTH"						=> true,
    "MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH"=> true,
    "MYSQLI_NOT_NULL_FLAG"				=> true,
    "MYSQLI_PRI_KEY_FLAG"				=> true,
    "MYSQLI_UNIQUE_KEY_FLAG"			=> true,
    "MYSQLI_MULTIPLE_KEY_FLAG"			=> true,
    "MYSQLI_BLOB_FLAG"					=> true,
    "MYSQLI_UNSIGNED_FLAG"				=> true,
    "MYSQLI_ZEROFILL_FLAG"				=> true,
    "MYSQLI_AUTO_INCREMENT_FLAG"		=> true,
    "MYSQLI_TIMESTAMP_FLAG"				=> true,
    "MYSQLI_SET_FLAG"					=> true,
    "MYSQLI_NUM_FLAG"					=> true,
    "MYSQLI_ENUM_FLAG"					=> true,
    "MYSQLI_BINARY_FLAG"				=> true,
    "MYSQLI_PART_KEY_FLAG"				=> true,
    "MYSQLI_GROUP_FLAG" 				=> true,
    "MYSQLI_SERVER_QUERY_NO_GOOD_INDEX_USED"=> true,
    "MYSQLI_SERVER_QUERY_NO_INDEX_USED"	=> true,
    "MYSQLI_OPT_LOAD_DATA_LOCAL_DIR"    => true,
    "MYSQLI_IS_MARIADB"                 => true,

    "MYSQLI_TYPE_DECIMAL"				=> true,
    "MYSQLI_TYPE_TINY"					=> true,
    "MYSQLI_TYPE_SHORT"					=> true,
    "MYSQLI_TYPE_LONG"					=> true,
    "MYSQLI_TYPE_FLOAT"					=> true,
    "MYSQLI_TYPE_DOUBLE"				=> true,
    "MYSQLI_TYPE_NULL"					=> true,
    "MYSQLI_TYPE_TIMESTAMP"				=> true,
    "MYSQLI_TYPE_LONGLONG"				=> true,
    "MYSQLI_TYPE_INT24"					=> true,
    "MYSQLI_TYPE_DATE"					=> true,
    "MYSQLI_TYPE_TIME"					=> true,
    "MYSQLI_TYPE_DATETIME"				=> true,
    "MYSQLI_TYPE_YEAR"					=> true,
    "MYSQLI_TYPE_NEWDATE"				=> true,
    "MYSQLI_TYPE_ENUM"					=> true,
    "MYSQLI_TYPE_SET"					=> true,
    "MYSQLI_TYPE_TINY_BLOB"				=> true,
    "MYSQLI_TYPE_MEDIUM_BLOB"			=> true,
    "MYSQLI_TYPE_LONG_BLOB"				=> true,
    "MYSQLI_TYPE_BLOB"					=> true,
    "MYSQLI_TYPE_VAR_STRING"			=> true,
    "MYSQLI_TYPE_STRING"				=> true,
    "MYSQLI_TYPE_CHAR"					=> true,
    "MYSQLI_TYPE_INTERVAL"				=> true,
    "MYSQLI_TYPE_GEOMETRY"				=> true,
    "MYSQLI_NO_DATA"					=> true,
    "MYSQLI_REPORT_INDEX"				=> true,
    "MYSQLI_REPORT_STRICT"				=> true,
    "MYSQLI_REPORT_ALL"					=> true,
    "MYSQLI_REPORT_ERROR"				=> true,
    "MYSQLI_REPORT_OFF"					=> true,
    "MYSQLI_SET_CHARSET_NAME"			=> true,
    "MYSQLI_SET_CHARSET_DIR"			=> true,
    "MYSQLI_REFRESH_GRANT"				=> true,
    "MYSQLI_REFRESH_LOG"				=> true,
    "MYSQLI_REFRESH_TABLES"				=> true,
    "MYSQLI_REFRESH_HOSTS"				=> true,
    "MYSQLI_REFRESH_STATUS"				=> true,
    "MYSQLI_REFRESH_THREADS"			=> true,
    "MYSQLI_REFRESH_REPLICA"			=> true,
    "MYSQLI_REFRESH_SLAVE"				=> true,
    "MYSQLI_REFRESH_MASTER"				=> true,
    "MYSQLI_DEBUG_TRACE_ENABLED"		=> true,
    "MYSQLI_TRANS_START_WITH_CONSISTENT_SNAPSHOT" => true,
    "MYSQLI_TRANS_START_READ_WRITE"		=> true,
    "MYSQLI_TRANS_START_READ_ONLY"		=> true,
    "MYSQLI_TRANS_COR_AND_CHAIN"		=> true,
    "MYSQLI_TRANS_COR_AND_NO_CHAIN"		=> true,
    "MYSQLI_TRANS_COR_RELEASE"			=> true,
    "MYSQLI_TRANS_COR_NO_RELEASE"		=> true,
);

$expected_constants['MYSQLI_OPT_INT_AND_FLOAT_NATIVE'] = true;

$expected_constants['MYSQLI_STORE_RESULT_COPY_DATA'] = true;

$expected_constants['MYSQLI_REFRESH_BACKUP_LOG'] = true;

$version = 50007 + 1;
$expected_constants['MYSQLI_OPT_NET_CMD_BUFFER_SIZE'] = true;
$expected_constants['MYSQLI_OPT_NET_READ_BUFFER_SIZE'] = true;
$expected_constants['MYSQLI_ASYNC'] = true;

$expected_constants['MYSQLI_SERVER_PS_OUT_PARAMS'] = true;

$expected_constants['MYSQLI_ON_UPDATE_NOW_FLAG'] = true;

$expected_constants['MYSQLI_SERVER_QUERY_WAS_SLOW'] = true;

$expected_constants['MYSQLI_CLIENT_SSL_VERIFY_SERVER_CERT'] = true;
$expected_constants['MYSQLI_CLIENT_SSL_DONT_VERIFY_SERVER_CERT'] = true;

$expected_constants['MYSQLI_SERVER_PUBLIC_KEY'] = true;

$expected_constants = array_merge($expected_constants, array(
    "MYSQLI_TYPE_NEWDECIMAL"	=> true,
    "MYSQLI_TYPE_BIT"			=> true,
));

$expected_constants['MYSQLI_NO_DEFAULT_VALUE_FLAG'] = true;

$expected_constants = array_merge($expected_constants, array(
    "MYSQLI_STMT_ATTR_CURSOR_TYPE"		=> true,
    "MYSQLI_CURSOR_TYPE_NO_CURSOR"		=> true,
    "MYSQLI_CURSOR_TYPE_READ_ONLY"		=> true,
    "MYSQLI_CURSOR_TYPE_FOR_UPDATE"		=> true,
    "MYSQLI_CURSOR_TYPE_SCROLLABLE"		=> true,
));

$expected_constants = array_merge($expected_constants, array(
    "MYSQLI_STMT_ATTR_PREFETCH_ROWS"	=> true,
));

$expected_constants['MYSQLI_OPT_SSL_VERIFY_SERVER_CERT'] = true;

/* pretty dump test, but that is the best way to mimic mysql.c */
$expected_constants["MYSQLI_DATA_TRUNCATED"] = true;

$expected_constants["MYSQLI_OPT_CAN_HANDLE_EXPIRED_PASSWORDS"] = true;
$expected_constants["MYSQLI_CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS"] = true;

$expected_constants["MYSQLI_TYPE_JSON"]	= true;

$unexpected_constants = array();

foreach ($constants as $group => $consts) {
    foreach ($consts as $name => $value) {
        if (stristr($name, 'mysqli')) {
            $name = strtoupper($name);
            if (isset($expected_constants[$name])) {
                unset($expected_constants[$name]);
            } else {
                $unexpected_constants[$name] = $name;
            }
        }
    }
}

if (!empty($unexpected_constants)) {
    printf("Dumping list of unexpected constants\n");
    var_dump($unexpected_constants);
}

if (!empty($expected_constants)) {
    printf("Dumping list of missing constants\n");
    var_dump($expected_constants);
}
?>
--EXPECT--
