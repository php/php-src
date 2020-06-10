<?php

/**
 * @param resource $statement_resource
 * @param string $column_name
 * @param mixed $variable
 * @param int $type
 * @return bool
 */
function oci_define_by_name($statement_resource, string $column_name, $variable, int $type = 0): bool {}

/**
 * @param resource $statement_resource
 * @param string $column_name
 * @param mixed $variable
 * @param int $maximum_length
 * @param int $type
 * @return bool
 */
function oci_bind_by_name($statement_resource, string $column_name, $variable, int $maximum_length = -1, int $type = 0): bool {}

/**
 * @param resource $statement_resource
 * @param string $column_name
 * @param mixed $variable
 * @param int $maximum_array_length
 * @param int $maximum_item_length
 * @param int $type
 * @return bool
 */
function oci_bind_array_by_name($statement_resource, string $column_name, $variable, int $maximum_array_length, int $maximum_item_length = -1, int $type = SQLT_AFC): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @return bool
 */
function oci_free_descriptor(OCI_Lob $lob_descriptor): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @param string $data
 * @param int $offset
 * @return bool
 */
function oci_lob_save(OCI_Lob $lob_descriptor, string $data, int $offset = 0): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @param $filename @todo path?
 * @return bool
 */
function oci_lob_import(OCI_Lob $lob_descriptor, $filename): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @return string|false @todo return false
 */
function oci_lob_load(OCI_Lob $lob_descriptor) {}

/**
 * @param OCI_Lob $lob_descriptor
 * @param int $length
 * @return string|false @todo return false
 */
function oci_lob_read(OCI_Lob $lob_descriptor, int $length) {}

/**
 * @param OCI_Lob $lob_descriptor
 * @return bool
 */
function oci_lob_eof(OCI_Lob $lob_descriptor): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @return int|false @todo return false
 */
function oci_lob_tell(OCI_Lob $lob_descriptor) {}

/**
 * @param OCI_Lob $lob_descriptor
 * @return bool
 */
function oci_lob_rewind(OCI_Lob $lob_descriptor): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @param int $offset
 * @param int $whence
 * @return bool
 */
function oci_lob_seek(OCI_Lob $lob_descriptor, int $offset, int $whence = 0): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @return int|false @todo return false
 */
function oci_lob_size(OCI_Lob $lob_descriptor) {}

/**
 * @param OCI_Lob $lob_descriptor
 * @param string $string
 * @param int $length
 * @return int|false @todo return false
 */
function oci_lob_write(OCI_Lob $lob_descriptor, string $string, int $length = 0) {}

/**
 * @param OCI_Lob $lob_descriptor_to
 * @param OCI_Lob $lob_descriptor_from
 * @return bool
 */
function oci_lob_append(OCI_Lob $lob_descriptor_to, OCI_Lob $lob_descriptor_from): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @param int $length
 * @return bool
 */
function oci_lob_truncate(OCI_Lob $lob_descriptor, int $length = 0): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @param int $offset
 * @param int $length
 * @return int|false @todo return false
 */
function oci_lob_erase(OCI_Lob $lob_descriptor, int $offset = -1, int $length = -1) {}

/**
 * @param OCI_Lob $lob_descriptor
 * @param int $flag
 * @return bool
 */
function oci_lob_flush(OCI_Lob $lob_descriptor, int $flag = 0): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @param bool $mode
 * @return bool
 */
function ocisetbufferinglob(OCI_Lob $lob_descriptor, bool $mode): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @return bool
 */
function ocigetbufferinglob(OCI_Lob $lob_descriptor): bool {}

/**
 * @param OCI_Lob $lob_descriptor_to
 * @param OCI_Lob $lob_descriptor_from
 * @param int $length
 * @return bool
 */
function oci_lob_copy(OCI_Lob $lob_descriptor_to, OCI_Lob $lob_descriptor_from, int $length = 0): bool {}

/**
 * @param OCI_Lob $lob_descriptor_first
 * @param OCI_Lob $lob_descriptor_second
 * @return bool
 */
function oci_lob_is_equal(OCI_Lob $lob_descriptor_first, OCI_Lob $lob_descriptor_second): bool {}

/**
 * @param OCI_Lob $lob_descriptor
 * @param $path @todo path?
 * @param int $start
 * @param int $length
 * @return bool
 */
function oci_lob_export(OCI_Lob $lob_descriptor, $path, int $start = -1, int $length = -1): bool {}

/**
 * @param resource $connection_resource
 * @param int $type
 * @return OCI_Lob
 */
function oci_new_descriptor($connection_resource, int $type = OCI_DTYPE_LOB): OCI_Lob {}

/**
 * @param resource $connection_resource
 * @return bool
 */
function oci_rollback($connection_resource): bool {}

/**
 * @param resource $connection_resource
 * @return bool
 */
function oci_commit($connection_resource): bool {}

/**
 * @param resource $statement_resource
 * @param mixed $column_number_or_name
 * @return string|false|null @todo return false, null?
 */
function oci_field_name($statement_resource, $column_number_or_name) {}

/**
 * @param resource $statement_resource
 * @param mixed $column_number_or_name
 * @return int|false|null @todo return false, null?
 */
function oci_field_size($statement_resource, $column_number_or_name) {}

/**
 * @param resource $statement_resource
 * @param mixed $column_number_or_name
 * @return int|false|null @todo return false, null?
 */
function oci_field_scale($statement_resource, $column_number_or_name) {}

/**
 * @param resource $statement_resource
 * @param mixed $column_number_or_name
 * @return int|false|null @todo return false, null?
 */
function oci_field_precision($statement_resource, $column_number_or_name) {}

/**
 * @param resource $statement_resource
 * @param mixed $column_number_or_name
 * @return string|int|false|null @todo return false, null?
 */
function oci_field_type($statement_resource, $column_number_or_name) {}

/**
 * @param resource $statement_resource
 * @param mixed $column_number_or_name
 * @return int|false|null @todo return false, null?
 */
function oci_field_type_raw($statement_resource, $column_number_or_name) {}

/**
 * @param resource $statement_resource
 * @param mixed $column_number_or_name
 * @return bool
 */
function oci_field_is_null($statement_resource, $column_number_or_name): bool {}

/**
 * @todo do we need this?
 * @param bool $mode
 * @return void
 */
function oci_internal_debug(bool $mode): void {}

/**
 * @param resource $statement_resource
 * @param int $mode
 * @return bool
 */
function oci_execute($statement_resource, int $mode = OCI_COMMIT_ON_SUCCESS): bool {}

/**
 * @param resource $statement_resource
 * @return bool
 */
function oci_cancel($statement_resource): bool {}

/**
 * @param resource $statement_resource
 * @return bool
 */
function oci_fetch($statement_resource): bool {}

/**
 * @param resource $statement_resource
 * @param mixed $result
 * @param int $mode
 * @return int|false @todo return false, double check?
 */
function ocifetchinto($statement_resource, $result, int $mode = 0) {}

/**
 * @param resource $statement_resource
 * @param mixed $output
 * @param int $skip
 * @param int $maximum_rows
 * @param int $flags
 * @return int @todo double check.
 */
function oci_fetch_all($statement_resource, $output, int $skip = 0, int $maximum_rows = -1, int $flags = 0): int {}

/**
 * @param resource $statement_resource
 * @param int $mode
 * @return object
 */
function oci_fetch_object($statement_resource, int $mode = 0): object {}

/**
 * @param resource $statement_resource
 * @return array|false @todo double check.
 */
function oci_fetch_row($statement_resource) {}

/**
 * @param resource $statement_resource
 * @return array|false @todo double check.
 */
function oci_fetch_assoc($statement_resource) {}

/**
 * @param resource $statement_resource
 * @param int $mode
 * @return array|false @todo double check.
 */
function oci_fetch_array($statement_resource, int $mode = 0) {}

/**
 * @param resource $statement_resource
 * @return bool
 */
function oci_free_statement($statement_resource): bool {}

/**
 * @param resource $connection_resource
 * @return bool
 */
function oci_close($connection_resource): bool {}

/**
 * @param string $username
 * @param string $password
 * @param string|null $connection_string
 * @param string|null $character_set
 * @param int $session_mode
 * @return resource|false @todo double check
 */
function oci_new_connect(string $username, string $password, ?string $connection_string = null, ?string $character_set = null, int $session_mode = OCI_DEFAULT) {}

/**
 * @param string $username
 * @param string $password
 * @param string|null $connection_string
 * @param string|null $character_set
 * @param int $session_mode
 * @return resource|false
 */
function oci_connect(string $username, string $password, ?string $connection_string = null, ?string $character_set = null, int $session_mode = OCI_DEFAULT) {}

/**
 * @param string $username
 * @param string $password
 * @param string|null $connection_string
 * @param string|null $character_set
 * @param int $session_mode
 * @return resource|false
 */
function oci_pconnect(string $username, string $password, ?string $connection_string = null, ?string $character_set = null, int $session_mode = OCI_DEFAULT) {}

/**
 * @param resource|null $connection_or_statement_resource
 * @return array|false @todo double check.
 */
function oci_error($connection_or_statement_resource = null) {}

/**
 * @param resource $statement_resource
 * @return int
 */
function oci_num_fields($statement_resource): int {}

/**
 * @param resource $connection_resource
 * @param string $sql_text
 * @return resource|false
 */
function oci_parse($connection_resource, string $sql_text) {}

/**
 * @param resource $statement_resource
 * @return resource|false
 */
function oci_get_implicit_resultset($statement_resource) {}

/**
 * @param resource $statement_resource
 * @param int $number_of_rows
 * @return bool
 */
function oci_set_prefetch($statement_resource, int $number_of_rows): bool {}

/**
 * @param resource $connection_resource
 * @param string $client_identifier
 * @return bool
 */
function oci_set_client_identifier($connection_resource, string $client_identifier): bool {}

/**
 * @param string $edition_name
 * @return bool
 */
function oci_set_edition(string $edition_name): bool {}

/**
 * @param resource $connection_resource
 * @param string $module_name
 * @return bool
 */
function oci_set_module_name($connection_resource, string $module_name): bool {}

/**
 * @param resource $connection_resource
 * @param string $action
 * @return bool
 */
function oci_set_action($connection_resource, string $action): bool {}

/**
 * @param resource $connection_resource
 * @param string $client_information
 * @return bool
 */
function oci_set_client_info($connection_resource, string $client_information): bool {}

/**
 * @param resource $connection_resource
 * @param string $action
 * @return bool
 */
function oci_set_db_operation($connection_resource, string $action): bool {}

/**
 * @param resource $connection_resource
 * @param int $call_timeout
 * @return bool
 */
function oci_set_call_timeout($connection_resource, int $call_timeout): bool {}

/**
 * @param resource|string $connection_resource_or_connection_string
 * @param string $username
 * @param string $old_password
 * @param string $new_password
 * @return resource|bool
 */
function oci_password_change($connection_resource_or_connection_string, string $username, string $old_password, string $new_password) {}

/**
 * @param resource $connection_resource
 * @return resource|false @todo check return false
 */
function oci_new_cursor($connection_resource) {}

/**
 * @param resource $statement_resource
 * @param mixed $column_number_or_name
 * @return mixed @todo double check.
 */
function oci_result($statement_resource, $column_number_or_name) {}

/**
 * @return string
 */
function oci_client_version(): string {}

/**
 * @param resource $connection_resource
 * @return string|false @todo check false
 */
function oci_server_version($connection_resource) {}

/**
 * @param resource $statement_resource
 * @return string|false @todo check false
 */
function oci_statement_type($statement_resource) {}

/**
 * @param resource $statement_resource
 * @return int|false @todo check false
 */
function oci_num_rows($statement_resource) {}

/**
 * @param OCI_Collection $collection
 * @return bool
 */
function oci_free_collection(OCI_Collection $collection): bool {}

/**
 * @param OCI_Collection $collection
 * @param string $value
 * @return bool
 */
function oci_collection_append(OCI_Collection $collection, string $value): bool {}

/**
 * @param OCI_Collection $collection
 * @param int $index
 * @return mixed @todo double check
 */
function oci_collection_element_get(OCI_Collection $collection, int $index) {}

/**
 * @param OCI_Collection $collection_to
 * @param OCI_Collection $collection_from
 * @return bool
 */
function oci_collection_assign(OCI_Collection $collection_to, OCI_Collection $collection_from): bool {}

/**
 * @param OCI_Collection $collection
 * @param int $index
 * @param string $value
 * @return bool
 */
function oci_collection_element_assign(OCI_Collection $collection, int $index, string $value): bool {}

/**
 * @param OCI_Collection $collection
 * @return int|false @otodo check return false.
 */
function oci_collection_size(OCI_Collection $collection) {}

/**
 * @param OCI_Collection $collection
 * @return int|false @otodo check return false.
 */
function oci_collection_max(OCI_Collection $collection) {}

/**
 * @param OCI_Collection $collection
 * @param int $number
 * @return bool
 */
function oci_collection_trim(OCI_Collection $collection, int $number): bool {}

/**
 * @param resource $connection_resource
 * @param string $type_name
 * @param string|null $schema_name
 * @return OCI_Collection|false @todo check return false.
 */
function oci_new_collection($connection_resource, string $type_name, ?string $schema_name = null) {}

/**
 * @param resource $connection_resource
 * @param mixed $function_name
 * @return bool @todo double check.
 */
function oci_register_taf_callback($connection_resource, $function_name): bool {}

/**
 * @param resource $connection_resource
 * @return bool
 */
function oci_unregister_taf_callback($connection_resource): bool {}

class OCI_Lob {
    /**
     * @param string $data
     * @param int $offset
     * @return bool
     */
    public function save(string $data, int $offset = 0): bool {}

    /**
     * @param $filename @todo path?
     * @return bool
     */
    public function import($filename): bool {}

    /**
     * @param $filename @todo path?
     * @return bool
     */
    public function savefile($filename): bool {}

    /**
     * @return string|false @todo return false
     */
    public function load() {}

    /**
     * @param int $length
     * @return string|false @todo return false
     */
    public function read(int $length) {}

    /**
     * @return bool
     */
    public function eof(): bool {}

    /**
     * @return int|false @todo return false
     */
    public function tell() {}

    /**
     * @return bool
     */
    public function rewind(): bool {}

    /**
     * @param int $offset
     * @param int $whence
     * @return bool
     */
    public function seek(int $offset, int $whence = 0): bool {}

    /**
     * @return int|false @todo return false
     */
    public function size() {}

    /**
     * @param string $string
     * @param int $length
     * @return int|false @todo return false
     */
    public function write(string $string, int $length = 0) {}

    /**
     * @param OCI_Lob $lob_descriptor_from
     * @return bool
     */
    public function append(OCI_Lob $lob_descriptor_from): bool {}

    /**
     * @param int $length
     * @return bool
     */
    public function truncate(int $length = 0): bool {}

    /**
     * @param int $offset
     * @param int $length
     * @return int|false @todo return false
     */
    public function erase(int $offset = -1, int $length = -1) {}

    /**
     * @param int $flag
     * @return bool
     */
    public function flush(int $flag = 0): bool {}

    /**
     * @param bool $mode
     * @return bool
     */
    public function setbuffering(bool $mode): bool {}

    /**
     * @return bool
     */
    public function getbuffering(): bool {}

    /**
     * @param $path @todo path?
     * @param int $start
     * @param int $length
     * @return bool
     */
    public function writetofile($path, int $start = -1, int $length = -1): bool {}

    /**
     * @param $path @todo path?
     * @param int $start
     * @param int $length
     * @return bool
     */
    public function export($path, int $start = -1, int $length = -1): bool {}

    /**
     * @param string $data
     * @param int $type
     * @return bool
     */
    public function writetemporary(string $data, int $type = OCI_TEMP_CLOB): bool {}

    /**
     * @return bool @todo double check.
     */
    public function close(): bool {}

    /**
     * @return bool
     */
    public function free(): bool {}
}

class OCI_Collection {
    /**
     * @return bool
     */
    public function free(): bool {}

    /**
     * @param string $value
     * @return bool
     */
    public function append(string $value): bool {}

    /**
     * @param int $index
     * @return mixed @todo double check
     */
    public function getElem(int $index) {}

    /**
     * @param OCI_Collection $collection_from
     * @return bool
     */
    public function assign(OCI_Collection $collection_from): bool {}

    /**
     * @param int $index
     * @param string $value
     * @return bool
     */
    public function assignelem(int $index, string $value): bool {}

    /**
     * @return int|false @otodo check return false.
     */
    public function size() {}

    /**
     * @return int|false @otodo check return false.
     */
    public function max() {}

    /**
     * @param int $number
     * @return bool
     */
    public function trim(int $number): bool {}
}