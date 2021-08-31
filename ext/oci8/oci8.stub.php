<?php

/** @generate-function-entries */

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

/** @return resource|false */
function oci_new_connect(string $username, string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 * @alias oci_new_connect
 * @deprecated
 */
function ocinlogon(string $username, string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 */
function oci_connect(string $username, string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 * @alias oci_connect
 * @deprecated
 */
function ocilogon(string $username, string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/** @return resource|false */
function oci_pconnect(string $username, string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 * @alias oci_pconnect
 * @deprecated
 */
function ociplogon(string $username, string $password, ?string $connection_string = null, string $encoding = "", int $session_mode = OCI_DEFAULT) {}

/** @param resource|null $connection_or_statement */
function oci_error($connection_or_statement = null): array|false {}

/**
 * @param resource|null $connection_or_statement
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

class OCILob {
    /**
     * @alias oci_lob_save
     * @return bool
     */
    public function save(string $data, int $offset = 0) {}

    /**
     * @alias oci_lob_import
     * @return bool
     */
    public function import(string $filename) {}

    /**
     * @alias oci_lob_import
     * @return bool
     */
    public function saveFile(string $filename) {}

    /**
     * @alias oci_lob_load
     * @return string|false
     */
    public function load() {}

    /**
     * @alias oci_lob_read
     * @return string|false
     */
    public function read(int $length) {}

    /**
     * @alias oci_lob_eof
     * @return bool
     */
    public function eof() {}

    /**
     * @alias oci_lob_tell
     * @return int|false
     */
    public function tell() {}

    /**
     * @alias oci_lob_rewind
     * @return bool
     */
    public function rewind() {}

    /**
     * @alias oci_lob_seek
     * @return bool
     */
    public function seek(int $offset, int $whence = OCI_SEEK_SET) {}

    /**
     * @alias oci_lob_size
     * @return int|false
     */
    public function size() {}

    /**
     * @alias oci_lob_write
     * @return int|false
     */
    public function write(string $data, ?int $length = null) {}

    /**
     * @alias oci_lob_append
     * @return bool
     */
    public function append(OCILob $from) {}

    /**
     * @alias oci_lob_truncate
     * @return bool
     */
    public function truncate(int $length = 0) {}

    /**
     * @alias oci_lob_erase
     * @return int|false
     */
    public function erase(?int $offset = null, ?int $length = null) {}

    /**
     * @alias oci_lob_flush
     * @return bool
     */
    public function flush(int $flag = 0): bool {}

    /**
     * @alias ocisetbufferinglob
     * @return bool
     */
    public function setBuffering(bool $mode) {}

    /**
     * @alias ocigetbufferinglob
     * @return bool
     */
    public function getBuffering() {}

    /**
     * @alias oci_lob_export
     * @return bool
     */
    public function writeToFile(string $filename, ?int $offset = null, ?int $length = null) {}

    /**
     * @alias oci_lob_export
     * @return bool
     */
    public function export(string $filename, ?int $offset = null, ?int $length = null) {}

    /** @return bool */
    public function writeTemporary(string $data, int $type = OCI_TEMP_CLOB) {}

    /** @return bool */
    public function close() {}

    /**
     * @alias oci_free_descriptor
     * @return bool
     */
    public function free() {}
}

class OCICollection {
    /**
     * @alias oci_free_collection
     * @return bool
     */
    public function free() {}

    /**
     * @alias oci_collection_append
     * @return bool
     */
    public function append(string $value) {}

    /**
     * @alias oci_collection_element_get
     * @return string|float|null|false
     */
    public function getElem(int $index) {}

    /**
     * @alias oci_collection_assign
     * @return bool
     */
    public function assign(OCICollection $from) {}

    /**
     * @alias oci_collection_element_assign
     * @return bool
     */
    public function assignElem(int $index, string $value) {}

    /**
     * @alias oci_collection_size
     * @return int|false
     */
    public function size() {}

    /**
     * @alias oci_collection_max
     * @return int|false
     */
    public function max() {}

    /**
     * @alias oci_collection_trim
     * @return bool
     */
    public function trim(int $num) {}
}
