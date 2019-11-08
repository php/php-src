<?php

function zend_version(): string {}

function func_num_args(): int {}

/** @return mixed */
function func_get_arg(int $arg_num) {}

function func_get_args(): array|false {}

function strlen(string $str): int {}

function strcmp(string $str1, string $str2): int {}

function strncmp(string $str1, string $str2, int $len): int|false {}

function error_reporting($new_error_level = UNKNOWN): int {}

function define(string $constant_name, $value, bool $case_insensitive = false): bool {}

function defined(string $constant_name): bool {}

function get_class(object $object = UNKNOWN): string|false {}

function get_called_class(): string|false {}

function get_parent_class($object = UNKNOWN): string|false {}

function is_subclass_of($object, string $class_name, bool $allow_string = true): bool {}

function is_a($object, string $class_name, bool $allow_string = false): bool {}

function get_class_vars(string $class_name): array|false {}

function get_object_vars(object $obj): array {}

function get_mangled_object_vars(object $obj): array {}

function get_class_methods($class): ?array {}

function method_exists($object_or_class, string $method): bool {}

function property_exists($object_or_class, string $property_name): ?bool {}

function class_exists(string $classname, bool $autoload = true): bool {}

function interface_exists(string $classname, bool $autoload = true): bool {}

function trait_exists(string $traitname, bool $autoload = true): bool {}

function function_exists(string $function_name): bool {}

function class_alias(string $user_class_name, string $alias_name, bool $autoload = true): bool {}

function get_included_files(): array {}

/** @alias get_included_files */
function get_required_files(): array {}

function trigger_error(string $message, int $error_type = E_USER_NOTICE): bool {}

/** @alias trigger_error */
function user_error(string $message, int $error_type = E_USER_NOTICE): bool {}

/** @return mixed */
function set_error_handler($error_handler, int $error_types = E_ALL) {}

function restore_error_handler(): bool {}

/** @return mixed */
function set_exception_handler($exception_handler) {}

function restore_exception_handler(): bool {}

function get_declared_classes(): array {}

function get_declared_traits(): array {}

function get_declared_interfaces(): array {}

function get_defined_functions(bool $exclude_disabled = false): array {}

function get_defined_vars(): ?array {}

function get_resource_type($res): string {}

function get_resources(string $type = UNKNOWN): array {}

function get_loaded_extensions(bool $zend_extensions = false): array {}

function get_defined_constants(bool $categorize = false): array {}

function debug_backtrace(int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT, int $limit = 0): array {}

function debug_print_backtrace(int $options = 0, int $limit = 0): void {}

function extension_loaded(string $extension_name): bool {}

function get_extension_funcs(string $extension_name): array|false {}

#if ZEND_DEBUG && defined(ZTS)
function zend_thread_id(): int {}
#endif

function gc_mem_caches(): int {}

function gc_collect_cycles(): int {}

function gc_enabled(): bool {}

function gc_enable(): void {}

function gc_disable(): void {}

function gc_status(): array {}
