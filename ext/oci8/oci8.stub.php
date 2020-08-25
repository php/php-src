<?php

/** @generate-function-entries */

/**
 * @param resource $statement_resource
 * @param mixed $variable
 */
function oci_define_by_name($statement_resource, string $column_name, &$variable, int $type = 0): bool {}

/**
 * @param resource $statement_resource
 * @param mixed $variable
 * @alias oci_define_by_name
 * @deprecated
 */
function ocidefinebyname($statement_resource, string $column_name, &$variable, int $type = 0): bool {}

/**
 * @param resource $statement_resource
 * @param mixed $variable
 */
function oci_bind_by_name($statement_resource, string $column_name, &$variable, int $maximum_length = -1, int $type = 0): bool {}

/**
 * @param resource $statement_resource
 * @param mixed $variable
 * @alias oci_bind_by_name
 * @deprecated
 */
function ocibindbyname($statement_resource, string $column_name, &$variable, int $maximum_length = -1, int $type = 0): bool {}

/**
 * @param resource $statement_resource
 * @param mixed $variable
 */
function oci_bind_array_by_name($statement_resource, string $column_name, &$variable, int $maximum_array_length, int $maximum_item_length = -1, int $type = SQLT_AFC): bool {}

function oci_free_descriptor(OCI_Lob $lob_descriptor): bool {}

/**
 * @alias oci_free_descriptor
 * @deprecated
 */
function ocifreedesc(OCI_Lob $lob_descriptor): bool {}

function oci_lob_save(OCI_Lob $lob_descriptor, string $data, int $offset = 0): bool {}

/**
 * @alias oci_lob_save
 * @deprecated
 */
function ocisavelob(OCI_Lob $lob_descriptor, string $data, int $offset = 0): bool {}

function oci_lob_import(OCI_Lob $lob_descriptor, string $filename): bool {}

/**
 * @alias oci_lob_import
 * @deprecated
 */
function ocisavelobfile(OCI_Lob $lob_descriptor, string $filename): bool {}

function oci_lob_load(OCI_Lob $lob_descriptor): string|false {}

/**
 * @alias oci_lob_load
 * @deprecated
 */
function ociloadlob(OCI_Lob $lob_descriptor): string|false {}

function oci_lob_read(OCI_Lob $lob_descriptor, int $length): string|false {}

function oci_lob_eof(OCI_Lob $lob_descriptor): bool {}

function oci_lob_tell(OCI_Lob $lob_descriptor): int|false {}

function oci_lob_rewind(OCI_Lob $lob_descriptor): bool {}

function oci_lob_seek(OCI_Lob $lob_descriptor, int $offset, int $whence = OCI_SEEK_SET): bool {}

function oci_lob_size(OCI_Lob $lob_descriptor): int|false {}

function oci_lob_write(OCI_Lob $lob_descriptor, string $string, int $length = UNKNOWN): int|false {}

function oci_lob_append(OCI_Lob $lob_descriptor_to, OCI_Lob $lob_descriptor_from): bool {}

function oci_lob_truncate(OCI_Lob $lob_descriptor, int $length = 0): bool {}

function oci_lob_erase(OCI_Lob $lob_descriptor, int $offset = UNKNOWN, int $length = UNKNOWN): int|false {}

function oci_lob_flush(OCI_Lob $lob_descriptor, int $flag = 0): bool {}

function ocisetbufferinglob(OCI_Lob $lob_descriptor, bool $mode): bool {}

function ocigetbufferinglob(OCI_Lob $lob_descriptor): bool {}

function oci_lob_copy(OCI_Lob $lob_descriptor_to, OCI_Lob $lob_descriptor_from, int $length = UNKNOWN): bool {}

function oci_lob_is_equal(OCI_Lob $lob_descriptor_first, OCI_Lob $lob_descriptor_second): bool {}

function oci_lob_export(OCI_Lob $lob_descriptor, string $path, int $start = UNKNOWN, int $length = UNKNOWN): bool {}

/**
 * @alias oci_lob_export
 * @deprecated
 */
function ociwritelobtofile(OCI_Lob $lob_descriptor, string $path, int $start = UNKNOWN, int $length = UNKNOWN): bool {}

/**
 * @param resource $connection_resource
 */
function oci_new_descriptor($connection_resource, int $type = OCI_DTYPE_LOB): ?OCI_Lob {}

/**
 * @param resource $connection_resource
 * @alias oci_new_descriptor
 * @deprecated
 */
function ocinewdescriptor($connection_resource, int $type = OCI_DTYPE_LOB): ?OCI_Lob {}

/**
 * @param resource $connection_resource
 */
function oci_rollback($connection_resource): bool {}

/**
 * @param resource $connection_resource
 * @alias oci_rollback
 * @deprecated
 */
function ocirollback($connection_resource): bool {}

/**
 * @param resource $connection_resource
 */
function oci_commit($connection_resource): bool {}

/**
 * @param resource $connection_resource
 * @alias oci_commit
 * @deprecated
 */
function ocicommit($connection_resource): bool {}

/**
 * @param resource $statement_resource
 */
function oci_field_name($statement_resource, string|int $column_number_or_name): string|false {}

/**
 * @param resource $statement_resource
 * @alias oci_field_name
 * @deprecated
 */
function ocicolumnname($statement_resource, string|int $column_number_or_name): string|false {}

/**
 * @param resource $statement_resource
 */
function oci_field_size($statement_resource, string|int $column_number_or_name): int|false {}

/**
 * @param resource $statement_resource
 * @alias oci_field_size
 * @deprecated
 */
function ocicolumnsize($statement_resource, string|int $column_number_or_name): int|false {}

/**
 * @param resource $statement_resource
 */
function oci_field_scale($statement_resource, string|int $column_number_or_name): int|false {}

/**
 * @param resource $statement_resource
 * @alias oci_field_scale
 * @deprecated
 */
function ocicolumnscale($statement_resource, string|int $column_number_or_name): int|false {}

/**
 * @param resource $statement_resource
 */
function oci_field_precision($statement_resource, string|int $column_number_or_name): int|false {}

/**
 * @param resource $statement_resource
 * @alias oci_field_precision
 * @deprecated
 */
function ocicolumnprecision($statement_resource, string|int $column_number_or_name): int|false {}

/**
 * @param resource $statement_resource
 */
function oci_field_type($statement_resource, string|int $column_number_or_name): string|int|false {}

/**
 * @param resource $statement_resource
 * @alias oci_field_type
 * @deprecated
 */
function ocicolumntype($statement_resource, string|int $column_number_or_name): string|int|false {}

/**
 * @param resource $statement_resource
 */
function oci_field_type_raw($statement_resource, string|int $column_number_or_name): int|false {}

/**
 * @param resource $statement_resource
 * @alias oci_field_type_raw
 * @deprecated
 */
function ocicolumntyperaw($statement_resource, string|int $column_number_or_name): int|false {}

/**
 * @param resource $statement_resource
 */
function oci_field_is_null($statement_resource, string|int $column_number_or_name): bool {}

/**
 * @param resource $statement_resource
 * @alias oci_field_is_null
 * @deprecated
 */
function ocicolumnisnull($statement_resource, string|int $column_number_or_name): bool {}

/**
 * @param resource $statement_resource
 */
function oci_execute($statement_resource, int $mode = OCI_COMMIT_ON_SUCCESS): bool {}

/**
 * @param resource $statement_resource
 * @alias oci_execute
 * @deprecated
 */
function ociexecute($statement_resource, int $mode = OCI_COMMIT_ON_SUCCESS): bool {}

/**
 * @param resource $statement_resource
 */
function oci_cancel($statement_resource): bool {}

/**
 * @param resource $statement_resource
 * @alias oci_cancel
 * @deprecated
 */
function ocicancel($statement_resource): bool {}

/**
 * @param resource $statement_resource
 */
function oci_fetch($statement_resource): bool {}

/**
 * @param resource $statement_resource
 * @alias oci_fetch
 * @deprecated
 */
function ocifetch($statement_resource): bool {}

/**
 * @param resource $statement_resource
 * @param array $result
 * @deprecated
 */
function ocifetchinto($statement_resource, &$result, int $mode = OCI_NUM): int|false {}

/**
 * @param resource $statement_resource
 * @param array $output
 */
function oci_fetch_all($statement_resource, &$output, int $skip = 0, int $maximum_rows = -1, int $flags = 0): int {}

/**
 * @param resource $statement_resource
 * @param mixed $output
 * @alias oci_fetch_all
 * @deprecated
 */
function ocifetchstatement($statement_resource, &$output, int $skip = 0, int $maximum_rows = -1, int $flags = 0): int {}

/**
 * @param resource $statement_resource
 */
function oci_fetch_object($statement_resource, int $mode = PHP_OCI_ASSOC | PHP_OCI_RETURN_NULLS): stdClass|null|false {}

/**
 * @param resource $statement_resource
 */
function oci_fetch_row($statement_resource): array|false {}

/**
 * @param resource $statement_resource
 */
function oci_fetch_assoc($statement_resource): array|false {}

/**
 * @param resource $statement_resource
 */
function oci_fetch_array($statement_resource, int $mode = PHP_OCI_BOTH | PHP_OCI_RETURN_NULLS): array|false {}

/**
 * @param resource $statement_resource
 */
function oci_free_statement($statement_resource): bool {}

/**
 * @param resource $statement_resource
 * @alias oci_free_statement
 * @deprecated
 */
function ocifreestatement($statement_resource): bool {}

/**
 * @param resource $statement_resource
 * @alias oci_free_statement
 */
function oci_free_cursor($statement_resource): bool {}

/**
 * @param resource $statement_resource
 * @alias oci_free_statement
 * @deprecated
 */
function ocifreecursor($statement_resource): bool {}

/**
 * @param resource $connection_resource
 */
function oci_close($connection_resource): bool|null {}

/**
 * @param resource $connection_resource
 * @alias oci_close
 * @deprecated
 */
function ocilogoff($connection_resource): bool|null {}

/**
 * @return resource|false
 */
function oci_new_connect(string $username, string $password, string $connection_string = UNKNOWN, string $character_set = '', int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 * @alias oci_new_connect
 * @deprecated
 */
function ocinlogon(string $username, string $password, string $connection_string = UNKNOWN, string $character_set = '', int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 */
function oci_connect(string $username, string $password, string $connection_string = UNKNOWN, string $character_set = '', int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 * @alias oci_connect
 * @deprecated
 */
function ocilogon(string $username, string $password, string $connection_string = UNKNOWN, string $character_set = '', int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 */
function oci_pconnect(string $username, string $password, string $connection_string = UNKNOWN, string $character_set = '', int $session_mode = OCI_DEFAULT) {}

/**
 * @return resource|false
 * @alias oci_pconnect
 * @deprecated
 */
function ociplogon(string $username, string $password, string $connection_string = UNKNOWN, string $character_set = '', int $session_mode = OCI_DEFAULT) {}

/**
 * @param resource $connection_or_statement_resource
 */
function oci_error($connection_or_statement_resource = UNKNOWN): array|false {}

/**
 * @param resource|null $connection_or_statement_resource
 * @alias oci_error
 * @deprecated
 */
function ocierror($connection_or_statement_resource = UNKNOWN): array|false {}

/**
 * @param resource $statement_resource
 */
function oci_num_fields($statement_resource): int {}

/**
 * @param resource $statement_resource
 * @alias oci_num_fields
 * @deprecated
 */
function ocinumcols($statement_resource): int {}

/**
 * @param resource $connection_resource
 * @return resource|false
 */
function oci_parse($connection_resource, string $sql_text) {}

/**
 * @param resource $connection_resource
 * @return resource|false
 * @alias oci_parse
 * @deprecated
 */
function ociparse($connection_resource, string $sql_text) {}

/**
 * @param resource $statement_resource
 * @return resource|false
 */
function oci_get_implicit_resultset($statement_resource) {}

/**
 * @param resource $statement_resource
 */
function oci_set_prefetch($statement_resource, int $number_of_rows): ?bool {}

/**
 * @param resource $statement_resource
 * @alias oci_set_prefetch
 * @deprecated
 */
function ocisetprefetch($statement_resource, int $number_of_rows): ?bool {}

/**
 * @param resource $connection_resource
 */
function oci_set_client_identifier($connection_resource, string $client_identifier): bool {}

function oci_set_edition(string $edition_name): bool {}

/**
 * @param resource $connection_resource
 */
function oci_set_module_name($connection_resource, string $module_name): bool {}

/**
 * @param resource $connection_resource
 */
function oci_set_action($connection_resource, string $action): bool {}

/**
 * @param resource $connection_resource
 */
function oci_set_client_info($connection_resource, string $client_information): bool {}

/**
 * @param resource $connection_resource
 */
function oci_set_db_operation($connection_resource, string $action): bool {}

/**
 * @param resource $connection_resource
 */
function oci_set_call_timeout($connection_resource, int $call_timeout): bool {}

/**
 * @param resource|string $connection_resource_or_connection_string
 * @return resource|bool
 */
function oci_password_change($connection_resource_or_connection_string, string $username, string $old_password, string $new_password) {}

/**
 * @param resource|string $connection_resource_or_connection_string
 * @return resource|bool
 * @alias oci_password_change
 * @deprecated
 */
function ocipasswordchange($connection_resource_or_connection_string, string $username, string $old_password, string $new_password) {}

/**
 * @param resource $connection_resource
 * @return resource|false
 */
function oci_new_cursor($connection_resource) {}

/**
 * @param resource $connection_resource
 * @return resource|false
 * @alias oci_new_cursor
 * @deprecated
 */
function ocinewcursor($connection_resource) {}

/**
 * @param resource $statement_resource
 */
function oci_result($statement_resource, string|int $column_number_or_name): mixed {}

/**
 * @param resource $statement_resource
 * @alias oci_result
 * @deprecated
 */
function ociresult($statement_resource, string|int $column_number_or_name): mixed {}

function oci_client_version(): string {}

/**
 * @param resource $connection_resource
 */
function oci_server_version($connection_resource): string|false {}

/**
 * @param resource $connection_resource
 * @alias oci_server_version
 * @deprecated
 */
function ociserverversion($connection_resource): string|false {}

/**
 * @param resource $statement_resource
 */
function oci_statement_type($statement_resource): string|false {}

/**
 * @param resource $statement_resource
 * @alias oci_statement_type
 * @deprecated
 */
function ocistatementtype($statement_resource): string|false {}

/**
 * @param resource $statement_resource
 */
function oci_num_rows($statement_resource): int|false {}

/**
 * @param resource $statement_resource
 * @alias oci_num_rows
 * @deprecated
 */
function ocirowcount($statement_resource): int|false {}

function oci_free_collection(OCI_Collection $collection): bool {}

/**
 * @alias oci_free_collection
 * @deprecated
 */
function ocifreecollection(OCI_Collection $collection): bool {}

function oci_collection_append(OCI_Collection $collection, string $value): bool {}

/**
 * @alias oci_collection_append
 * @deprecated
 */
function ocicollappend(OCI_Collection $collection, string $value): bool {}

function oci_collection_element_get(OCI_Collection $collection, int $index): string|float|null|false {}

/**
 * @alias oci_collection_element_get
 * @deprecated
 */
function ocicollgetelem(OCI_Collection $collection, int $index): string|float|null|false {}

function oci_collection_assign(OCI_Collection $collection_to, OCI_Collection $collection_from): bool {}

function oci_collection_element_assign(OCI_Collection $collection, int $index, string $value): bool {}

/**
 * @alias oci_collection_element_assign
 * @deprecated
 */
function ocicollassignelem(OCI_Collection $collection, int $index, string $value): bool {}

function oci_collection_size(OCI_Collection $collection): int|false {}

/**
 * @alias oci_collection_size
 * @deprecated
 */
function ocicollsize(OCI_Collection $collection): int|false {}

function oci_collection_max(OCI_Collection $collection): int|false {}

/**
 * @alias oci_collection_max
 * @deprecated
 */
function ocicollmax(OCI_Collection $collection): int|false {}

function oci_collection_trim(OCI_Collection $collection, int $number): bool {}

/**
 * @alias oci_collection_trim
 * @deprecated
 */
function ocicolltrim(OCI_Collection $collection, int $number): bool {}

/**
 * @param resource $connection_resource
 */
function oci_new_collection($connection_resource, string $type_name, string $schema_name = UNKNOWN): OCI_Collection|false {}

/**
 * @param resource $connection_resource
 * @alias oci_new_collection
 * @deprecated
 */
function ocinewcollection($connection_resource, string $type_name, string $schema_name = UNKNOWN): OCI_Collection|false {}

/**
 * @param resource $connection_resource
 */
function oci_register_taf_callback($connection_resource, ?callable $function_name): bool {}

/**
 * @param resource $connection_resource
 */
function oci_unregister_taf_callback($connection_resource): bool {}

class OCI_Lob {
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
    public function savefile(string $filename) {}

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
    public function write(string $string, int $length = UNKNOWN) {}

    /**
     * @alias oci_lob_append
     * @return bool
     */
    public function append(OCI_Lob $lob_descriptor_from) {}

    /**
     * @alias oci_lob_truncate
     * @return bool
     */
    public function truncate(int $length = 0) {}

    /**
     * @alias oci_lob_erase
     * @return int|false
     */
    public function erase(int $offset = UNKNOWN, int $length = UNKNOWN) {}

    /**
     * @alias oci_lob_flush
     * @return bool
     */
    public function flush(int $flag = 0): bool {}

    /**
     * @alias ocisetbufferinglob
     * @return bool
     */
    public function setbuffering(bool $mode) {}

    /**
     * @alias ocigetbufferinglob
     * @return bool
     */
    public function getbuffering() {}

    /**
     * @alias oci_lob_export
     * @return bool
     */
    public function writetofile(string $path, int $start = UNKNOWN, int $length = UNKNOWN) {}

    /**
     * @alias oci_lob_export
     * @return bool
     */
    public function export(string $path, int $start = UNKNOWN, int $length = UNKNOWN) {}

    /**
     * @alias oci_lob_write_temporary
     * @return bool
     */
    public function writetemporary(string $data, int $type = OCI_TEMP_CLOB) {}

    /**
     * @alias oci_lob_close
     * @return bool
     */
    public function close() {}

    /**
     * @alias oci_free_descriptor
     * @return bool
     */
    public function free() {}
}

class OCI_Collection {
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
    public function assign(OCI_Collection $collection_from) {}

    /**
     * @alias oci_collection_element_assign
     * @return bool
     */
    public function assignelem(int $index, string $value) {}

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
    public function trim(int $number) {}
}
