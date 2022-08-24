<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue OCI_DEFAULT
 */
const OCI_DEFAULT = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_SYSOPER
 */
const OCI_SYSOPER = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_SYSDBA
 */
const OCI_SYSDBA = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OCI_CRED_EXT
 */
const OCI_CRED_EXT = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_DESCRIBE_ONLY
 */
const OCI_DESCRIBE_ONLY = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_COMMIT_ON_SUCCESS
 */
const OCI_COMMIT_ON_SUCCESS = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_DEFAULT
 */
const OCI_NO_AUTO_COMMIT = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_EXACT_FETCH
 */
const OCI_EXACT_FETCH = UNKNOWN;

/* for $LOB->seek() */

/**
 * @var int
 * @cvalue PHP_OCI_SEEK_SET
 */
const OCI_SEEK_SET = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OCI_SEEK_CUR
 */
const OCI_SEEK_CUR = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OCI_SEEK_END
 */
const OCI_SEEK_END = UNKNOWN;

/*	for $LOB->flush() */

/**
 * @var int
 * @cvalue OCI_LOB_BUFFER_FREE
 */
const OCI_LOB_BUFFER_FREE = UNKNOWN;

/* for OCIBindByName (real "oci" names + short "php" names */

/**
 * @var int
 * @cvalue SQLT_BFILEE
 */
const SQLT_BFILEE = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_CFILEE
 */
const SQLT_CFILEE = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_CLOB
 */
const SQLT_CLOB = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_BLOB
 */
const SQLT_BLOB = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_RDD
 */
const SQLT_RDD = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_INT
 */
const SQLT_INT = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_NUM
 */
const SQLT_NUM = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_RSET
 */
const SQLT_RSET = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_AFC
 */
const SQLT_AFC = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_CHR
 */
const SQLT_CHR = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_VCS
 */
const SQLT_VCS = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_AVC
 */
const SQLT_AVC = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_STR
 */
const SQLT_STR = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_LVC
 */
const SQLT_LVC = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_FLT
 */
const SQLT_FLT = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_UIN
 */
const SQLT_UIN = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_LNG
 */
const SQLT_LNG = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_LBI
 */
const SQLT_LBI = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_BIN
 */
const SQLT_BIN = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_ODT
 */
const SQLT_ODT = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_BDOUBLE
 */
const SQLT_BDOUBLE = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_BFLOAT
 */
const SQLT_BFLOAT = UNKNOWN;
#if OCI_MAJOR_VERSION >= 12
/**
 * @var int
 * @cvalue SQLT_BOL
 */
const SQLT_BOL = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue SQLT_NTY
 */
const OCI_B_NTY = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_NTY
 */
const SQLT_NTY = UNKNOWN;

/** @var string */
const OCI_SYSDATE = "SYSDATE";

/**
 * @var int
 * @cvalue SQLT_BFILEE
 */
const OCI_B_BFILE = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_CFILEE
 */
const OCI_B_CFILEE = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_CLOB
 */
const OCI_B_CLOB = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_BLOB
 */
const OCI_B_BLOB = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_RDD
 */
const OCI_B_ROWID = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_RSET
 */
const OCI_B_CURSOR = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_BIN
 */
const OCI_B_BIN = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_INT
 */
const OCI_B_INT = UNKNOWN;
/**
 * @var int
 * @cvalue SQLT_NUM
 */
const OCI_B_NUM = UNKNOWN;
#if OCI_MAJOR_VERSION >= 12
/**
 * @var int
 * @cvalue SQLT_BOL
 */
const OCI_B_BOL = UNKNOWN;
#endif

/* for OCIFetchStatement */

/**
 * @var int
 * @cvalue PHP_OCI_FETCHSTATEMENT_BY_COLUMN
 */
const OCI_FETCHSTATEMENT_BY_COLUMN = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OCI_FETCHSTATEMENT_BY_ROW
 */
const OCI_FETCHSTATEMENT_BY_ROW = UNKNOWN;

/* for OCIFetchInto & OCIResult */

/**
 * @var int
 * @cvalue PHP_OCI_ASSOC
 */
const OCI_ASSOC = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OCI_NUM
 */
const OCI_NUM = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OCI_BOTH
 */
const OCI_BOTH = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OCI_RETURN_NULLS
 */
const OCI_RETURN_NULLS = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OCI_RETURN_LOBS
 */
const OCI_RETURN_LOBS = UNKNOWN;

/* for OCINewDescriptor (real "oci" names + short "php" names */

/**
 * @var int
 * @cvalue OCI_DTYPE_FILE
 */
const OCI_DTYPE_FILE = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_DTYPE_LOB
 */
const OCI_DTYPE_LOB = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_DTYPE_ROWID
 */
const OCI_DTYPE_ROWID = UNKNOWN;

/**
 * @var int
 * @cvalue OCI_DTYPE_FILE
 */
const OCI_D_FILE = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_DTYPE_LOB
 */
const OCI_D_LOB = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_DTYPE_ROWID
 */
const OCI_D_ROWID = UNKNOWN;

/* for OCIWriteTemporaryLob */

/**
 * @var int
 * @cvalue OCI_TEMP_CLOB
 */
const OCI_TEMP_CLOB = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_TEMP_BLOB
 */
const OCI_TEMP_BLOB = UNKNOWN;

/* for Transparent Application Failover */

/**
 * @var int
 * @cvalue OCI_FO_END
 */
const OCI_FO_END = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_FO_ABORT
 */
const OCI_FO_ABORT = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_FO_REAUTH
 */
const OCI_FO_REAUTH = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_FO_BEGIN
 */
const OCI_FO_BEGIN = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_FO_ERROR
 */
const OCI_FO_ERROR = UNKNOWN;

/**
 * @var int
 * @cvalue OCI_FO_NONE
 */
const OCI_FO_NONE = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_FO_SESSION
 */
const OCI_FO_SESSION = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_FO_SELECT
 */
const OCI_FO_SELECT = UNKNOWN;
/**
 * @var int
 * @cvalue OCI_FO_TXNAL
 */
const OCI_FO_TXNAL = UNKNOWN;

/**
 * @var int
 * @cvalue OCI_FO_RETRY
 */
const OCI_FO_RETRY = UNKNOWN;

/** @param resource $statement */
function oci_define_by_name($statement, string $column, mixed &$var, int $type = 0): bool {}

/**
 * @param resource $statement
 * @alias oci_define_by_name
 * @deprecated
 */
function ocidefinebyname($statement, string $column, mixed &$var, int $type = 0): bool {}

/** @param resource $statement */
function oci_bind_by_name($statement, string $param, mixed &$var, int $max_length = -1, int $type = 0): bool {}

/**
 * @param resource $statement
 * @alias oci_bind_by_name
 * @deprecated
 */
function ocibindbyname($statement, string $param, mixed &$var, int $max_length = -1, int $type = 0): bool {}

/**
 * @param resource $statement
 * @param array $var
 */
function oci_bind_array_by_name($statement, string $param, &$var, int $max_array_length, int $max_item_length = -1, int $type = SQLT_AFC): bool {}

function oci_free_descriptor(OCILob $lob): bool {}

/**
 * @alias oci_free_descriptor
 * @deprecated
 */
function ocifreedesc(OCILob $lob): bool {}

function oci_lob_save(OCILob $lob, string $data, int $offset = 0): bool {}

/**
 * @alias oci_lob_save
 * @deprecated
 */
function ocisavelob(OCILob $lob, string $data, int $offset = 0): bool {}

function oci_lob_import(OCILob $lob, string $filename): bool {}

/**
 * @alias oci_lob_import
 * @deprecated
 */
function ocisavelobfile(OCILob $lob, string $filename): bool {}

function oci_lob_load(OCILob $lob): string|false {}

/**
 * @alias oci_lob_load
 * @deprecated
 */
function ociloadlob(OCILob $lob): string|false {}

function oci_lob_read(OCILob $lob, int $length): string|false {}

function oci_lob_eof(OCILob $lob): bool {}

function oci_lob_tell(OCILob $lob): int|false {}

function oci_lob_rewind(OCILob $lob): bool {}

function oci_lob_seek(OCILob $lob, int $offset, int $whence = OCI_SEEK_SET): bool {}

function oci_lob_size(OCILob $lob): int|false {}

function oci_lob_write(OCILob $lob, string $data, ?int $length = null): int|false {}

function oci_lob_append(OCILob $to, OCILob $from): bool {}

function oci_lob_truncate(OCILob $lob, int $length = 0): bool {}

function oci_lob_erase(OCILob $lob, ?int $offset = null, ?int $length = null): int|false {}

function oci_lob_flush(OCILob $lob, int $flag = 0): bool {}

function ocisetbufferinglob(OCILob $lob, bool $mode): bool {}

function ocigetbufferinglob(OCILob $lob): bool {}

function oci_lob_copy(OCILob $to, OCILob $from, ?int $length = null): bool {}

function oci_lob_is_equal(OCILob $lob1, OCILob $lob2): bool {}

function oci_lob_export(OCILob $lob, string $filename, ?int $offset = null, ?int $length = null): bool {}

/**
 * @alias oci_lob_export
 * @deprecated
 */
function ociwritelobtofile(OCILob $lob, string $filename, ?int $offset = null, ?int $length = null): bool {}

/** @param resource $connection */
function oci_new_descriptor($connection, int $type = OCI_DTYPE_LOB): ?OCILob {}

/**
 * @param resource $connection
 * @alias oci_new_descriptor
 * @deprecated
 */
function ocinewdescriptor($connection, int $type = OCI_DTYPE_LOB): ?OCILob {}

/** @param resource $connection */
function oci_rollback($connection): bool {}

/**
 * @param resource $connection
 * @alias oci_rollback
 * @deprecated
 */
function ocirollback($connection): bool {}

/** @param resource $connection */
function oci_commit($connection): bool {}

/**
 * @param resource $connection
 * @alias oci_commit
 * @deprecated
 */
function ocicommit($connection): bool {}

/** @param resource $statement */
function oci_field_name($statement, string|int $column): string|false {}

/**
 * @param resource $statement
 * @alias oci_field_name
 * @deprecated
 */
function ocicolumnname($statement, string|int $column): string|false {}

/** @param resource $statement */
function oci_field_size($statement, string|int $column): int|false {}

/**
 * @param resource $statement
 * @alias oci_field_size
 * @deprecated
 */
function ocicolumnsize($statement, string|int $column): int|false {}

/** @param resource $statement */
function oci_field_scale($statement, string|int $column): int|false {}

/**
 * @param resource $statement
 * @alias oci_field_scale
 * @deprecated
 */
function ocicolumnscale($statement, string|int $column): int|false {}

/** @param resource $statement */
function oci_field_precision($statement, string|int $column): int|false {}

/**
 * @param resource $statement
 * @alias oci_field_precision
 * @deprecated
 */
function ocicolumnprecision($statement, string|int $column): int|false {}

/** @param resource $statement */
function oci_field_type($statement, string|int $column): string|int|false {}

/**
 * @param resource $statement
 * @alias oci_field_type
 * @deprecated
 */
function ocicolumntype($statement, string|int $column): string|int|false {}

/** @param resource $statement */
function oci_field_type_raw($statement, string|int $column): int|false {}

/**
 * @param resource $statement
 * @alias oci_field_type_raw
 * @deprecated
 */
function ocicolumntyperaw($statement, string|int $column): int|false {}

/** @param resource $statement */
function oci_field_is_null($statement, string|int $column): bool {}

/**
 * @param resource $statement
 * @alias oci_field_is_null
 * @deprecated
 */
function ocicolumnisnull($statement, string|int $column): bool {}

/** @param resource $statement */
function oci_execute($statement, int $mode = OCI_COMMIT_ON_SUCCESS): bool {}

/**
 * @param resource $statement
 * @alias oci_execute
 * @deprecated
 */
function ociexecute($statement, int $mode = OCI_COMMIT_ON_SUCCESS): bool {}

/** @param resource $statement */
function oci_cancel($statement): bool {}

/**
 * @param resource $statement
 * @alias oci_cancel
 * @deprecated
 */
function ocicancel($statement): bool {}

/** @param resource $statement */
function oci_fetch($statement): bool {}

/**
 * @param resource $statement
 * @alias oci_fetch
 * @deprecated
 */
function ocifetch($statement): bool {}

/**
 * @param resource $statement
 * @param array $result
 * @deprecated
 */
function ocifetchinto($statement, &$result, int $mode = OCI_NUM): int|false {}

/**
 * @param resource $statement
 * @param array $output
 */
function oci_fetch_all($statement, &$output, int $offset = 0, int $limit = -1, int $flags = OCI_FETCHSTATEMENT_BY_COLUMN | OCI_ASSOC): int {}

/**
 * @param resource $statement
 * @param array $output
 * @alias oci_fetch_all
 * @deprecated
 */
function ocifetchstatement($statement, &$output, int $offset = 0, int $limit = -1, int $flags = OCI_FETCHSTATEMENT_BY_COLUMN | OCI_ASSOC): int {}

/** @param resource $statement */
function oci_fetch_object($statement, int $mode = OCI_ASSOC | OCI_RETURN_NULLS): stdClass|false {}

/** @param resource $statement */
function oci_fetch_row($statement): array|false {}

/** @param resource $statement */
function oci_fetch_assoc($statement): array|false {}

/** @param resource $statement */
function oci_fetch_array($statement, int $mode = OCI_BOTH | OCI_RETURN_NULLS): array|false {}

/** @param resource $statement */
function oci_free_statement($statement): bool {}

/**
 * @param resource $statement
 * @alias oci_free_statement
 * @deprecated
 */
function ocifreestatement($statement): bool {}

/**
 * @param resource $statement
 * @alias oci_free_statement
 */
function oci_free_cursor($statement): bool {}

/**
 * @param resource $statement
 * @alias oci_free_statement
 * @deprecated
 */
function ocifreecursor($statement): bool {}

/** @param resource $connection */
function oci_close($connection): ?bool {}

/**
 * @param resource $connection
 * @alias oci_close
 * @deprecated
 */
function ocilogoff($connection): ?bool {}

/**
 * @return resource|false
 */
function oci_new_connect(string $username, #[\SensitiveParameter] string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 * @alias oci_new_connect
 * @deprecated
 */
function ocinlogon(string $username, #[\SensitiveParameter] string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 */
function oci_connect(string $username, #[\SensitiveParameter] string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 * @alias oci_connect
 * @deprecated
 */
function ocilogon(string $username, #[\SensitiveParameter] string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 */
function oci_pconnect(string $username, #[\SensitiveParameter] string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 * @alias oci_pconnect
 * @deprecated
 */
function ociplogon(string $username, #[\SensitiveParameter] string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/**
 * @param resource|null $connection_or_statement
 * @return array<string, int|string>|false
 * @refcount 1
 */
function oci_error($connection_or_statement = null): array|false {}

/**
 * @param resource|null $connection_or_statement
 * @return array<string, int|string>|false
 * @alias oci_error
 * @deprecated
 */
function ocierror($connection_or_statement = null): array|false {}

/** @param resource $statement */
function oci_num_fields($statement): int {}

/**
 * @param resource $statement
 * @alias oci_num_fields
 * @deprecated
 */
function ocinumcols($statement): int {}

/**
 * @param resource $connection
 * @return resource|false
 */
function oci_parse($connection, string $sql) {}

/**
 * @param resource $connection
 * @return resource|false
 * @alias oci_parse
 * @deprecated
 */
function ociparse($connection, string $sql) {}

/**
 * @param resource $statement
 * @return resource|false
 */
function oci_get_implicit_resultset($statement) {}

/** @param resource $statement */
function oci_set_prefetch($statement, int $rows): bool {}

/**
 * @param resource $statement
 * @alias oci_set_prefetch
 * @deprecated
 */
function ocisetprefetch($statement, int $rows): bool {}

/** @param resource $statement */
function oci_set_prefetch_lob($statement, int $prefetch_lob_size): bool {}

/** @param resource $connection */
function oci_set_client_identifier($connection, string $client_id): bool {}

function oci_set_edition(string $edition): bool {}

/** @param resource $connection */
function oci_set_module_name($connection, string $name): bool {}

/** @param resource $connection */
function oci_set_action($connection, string $action): bool {}

/**
 * @param resource $connection
 */
function oci_set_client_info($connection, string $client_info): bool {}

/** @param resource $connection */
function oci_set_db_operation($connection, string $action): bool {}

/** @param resource $connection */
function oci_set_call_timeout($connection, int $timeout): bool {}

/**
 * @param resource|string $connection
 * @return resource|bool
 */
function oci_password_change($connection, string $username, string $old_password, string $new_password) {}

/**
 * @param resource|string $connection
 * @return resource|bool
 * @alias oci_password_change
 * @deprecated
 */
function ocipasswordchange($connection, string $username, string $old_password, string $new_password) {}

/**
 * @param resource $connection
 * @return resource|false
 */
function oci_new_cursor($connection) {}

/**
 * @param resource $connection
 * @return resource|false
 * @alias oci_new_cursor
 * @deprecated
 */
function ocinewcursor($connection) {}

/** @param resource $statement */
function oci_result($statement, string|int $column): mixed {}

/**
 * @param resource $statement
 * @alias oci_result
 * @deprecated
 */
function ociresult($statement, string|int $column): mixed {}

function oci_client_version(): string {}

/** @param resource $connection */
function oci_server_version($connection): string|false {}

/**
 * @param resource $connection
 * @alias oci_server_version
 * @deprecated
 */
function ociserverversion($connection): string|false {}

/** @param resource $statement */
function oci_statement_type($statement): string|false {}

/**
 * @param resource $statement
 * @alias oci_statement_type
 * @deprecated
 */
function ocistatementtype($statement): string|false {}

/** @param resource $statement */
function oci_num_rows($statement): int|false {}

/**
 * @param resource $statement
 * @alias oci_num_rows
 * @deprecated
 */
function ocirowcount($statement): int|false {}

function oci_free_collection(OCICollection $collection): bool {}

/**
 * @alias oci_free_collection
 * @deprecated
 */
function ocifreecollection(OCICollection $collection): bool {}

function oci_collection_append(OCICollection $collection, string $value): bool {}

/**
 * @alias oci_collection_append
 * @deprecated
 */
function ocicollappend(OCICollection $collection, string $value): bool {}

function oci_collection_element_get(OCICollection $collection, int $index): string|float|null|false {}

/**
 * @alias oci_collection_element_get
 * @deprecated
 */
function ocicollgetelem(OCICollection $collection, int $index): string|float|null|false {}

function oci_collection_assign(OCICollection $to, OCICollection $from): bool {}

function oci_collection_element_assign(OCICollection $collection, int $index, string $value): bool {}

/**
 * @alias oci_collection_element_assign
 * @deprecated
 */
function ocicollassignelem(OCICollection $collection, int $index, string $value): bool {}

function oci_collection_size(OCICollection $collection): int|false {}

/**
 * @alias oci_collection_size
 * @deprecated
 */
function ocicollsize(OCICollection $collection): int|false {}

function oci_collection_max(OCICollection $collection): int|false {}

/**
 * @alias oci_collection_max
 * @deprecated
 */
function ocicollmax(OCICollection $collection): int|false {}

function oci_collection_trim(OCICollection $collection, int $num): bool {}

/**
 * @alias oci_collection_trim
 * @deprecated
 */
function ocicolltrim(OCICollection $collection, int $num): bool {}

/** @param resource $connection */
function oci_new_collection($connection, string $type_name, ?string $schema = null): OCICollection|false {}

/**
 * @param resource $connection
 * @alias oci_new_collection
 * @deprecated
 */
function ocinewcollection($connection, string $type_name, ?string $schema = null): OCICollection|false {}

/** @param resource $connection */
function oci_register_taf_callback($connection, ?callable $callback): bool {}

/** @param resource $connection */
function oci_unregister_taf_callback($connection): bool {}

#[\AllowDynamicProperties]
class OCILob {
    /**
     * @alias oci_lob_save
     * @tentative-return-type
     */
    public function save(string $data, int $offset = 0): bool {}

    /**
     * @alias oci_lob_import
     * @tentative-return-type
     */
    public function import(string $filename): bool {}

    /**
     * @alias oci_lob_import
     * @tentative-return-type
     */
    public function saveFile(string $filename): bool {}

    /**
     * @alias oci_lob_load
     * @tentative-return-type
     */
    public function load(): string|false {}

    /**
     * @alias oci_lob_read
     * @tentative-return-type
     */
    public function read(int $length): string|false {}

    /**
     * @alias oci_lob_eof
     * @tentative-return-type
     */
    public function eof(): bool {}

    /**
     * @alias oci_lob_tell
     * @tentative-return-type
     */
    public function tell(): int|false {}

    /**
     * @alias oci_lob_rewind
     * @tentative-return-type
     */
    public function rewind(): bool {}

    /**
     * @alias oci_lob_seek
     * @tentative-return-type
     */
    public function seek(int $offset, int $whence = OCI_SEEK_SET): bool {}

    /**
     * @alias oci_lob_size
     * @tentative-return-type
     */
    public function size(): int|false {}

    /**
     * @alias oci_lob_write
     * @tentative-return-type
     */
    public function write(string $data, ?int $length = null): int|false {}

    /**
     * @alias oci_lob_append
     * @tentative-return-type
     */
    public function append(OCILob $from): bool {}

    /**
     * @alias oci_lob_truncate
     * @tentative-return-type
     */
    public function truncate(int $length = 0): bool {}

    /**
     * @alias oci_lob_erase
     * @tentative-return-type
     */
    public function erase(?int $offset = null, ?int $length = null): int|false {}

    /**
     * @alias oci_lob_flush
     * @tentative-return-type
     */
    public function flush(int $flag = 0): bool {}

    /**
     * @alias ocisetbufferinglob
     * @tentative-return-type
     */
    public function setBuffering(bool $mode): bool {}

    /**
     * @alias ocigetbufferinglob
     * @tentative-return-type
     */
    public function getBuffering(): bool {}

    /**
     * @alias oci_lob_export
     * @tentative-return-type
     */
    public function writeToFile(string $filename, ?int $offset = null, ?int $length = null): bool {}

    /**
     * @alias oci_lob_export
     * @tentative-return-type
     */
    public function export(string $filename, ?int $offset = null, ?int $length = null): bool {}

    /** @tentative-return-type */
    public function writeTemporary(string $data, int $type = OCI_TEMP_CLOB): bool {}

    /** @tentative-return-type */
    public function close(): bool {}

    /**
     * @alias oci_free_descriptor
     * @tentative-return-type
     */
    public function free(): bool {}
}

#[\AllowDynamicProperties]
class OCICollection {
    /**
     * @alias oci_free_collection
     * @tentative-return-type
     */
    public function free(): bool {}

    /**
     * @alias oci_collection_append
     * @tentative-return-type
     */
    public function append(string $value): bool {}

    /**
     * @alias oci_collection_element_get
     * @tentative-return-type
     */
    public function getElem(int $index): string|float|null|false {}

    /**
     * @alias oci_collection_assign
     * @tentative-return-type
     */
    public function assign(OCICollection $from): bool {}

    /**
     * @alias oci_collection_element_assign
     * @tentative-return-type
     */
    public function assignElem(int $index, string $value): bool {}

    /**
     * @alias oci_collection_size
     * @tentative-return-type
     */
    public function size(): int|false {}

    /**
     * @alias oci_collection_max
     * @tentative-return-type
     */
    public function max(): int|false {}

    /**
     * @alias oci_collection_trim
     * @tentative-return-type
     */
    public function trim(int $num): bool {}
}
