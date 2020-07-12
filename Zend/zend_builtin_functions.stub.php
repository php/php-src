<?php

/** @generate-function-entries */

function zend_version(): string {}

function func_num_args(): int {}

function func_get_arg(int $argument_number): mixed {}

function func_get_args(): array {}

function strlen(string $string): int {}

function strcmp(string $string1, string $string2): int {}

function strncmp(string $string1, string $string2, int $length): int {}

function strcasecmp(string $string1, string $string2): int {}

function strncasecmp(string $string1, string $string2, int $length): int {}

function error_reporting(?int $error_level = null): int {}

/** @param bool|int|float|string|array|resource|null $value */
function define(string $constant_name, $value): bool {}

function defined(string $constant_name): bool {}

function get_class(object $object = UNKNOWN): string {}

function get_called_class(): string {}

function get_parent_class(string|object $object = UNKNOWN): string|false {}

function is_subclass_of($object, string $class_name, bool $allow_string = true): bool {}

function is_a($object, string $class_name, bool $allow_string = false): bool {}

function get_class_vars(string $class_name): array|false {}

function get_object_vars(object $object): array {}

function get_mangled_object_vars(object $object): array {}

/** string|object $class */
function get_class_methods($class): array {}

/** @param object|string $object_or_class */
function method_exists($object_or_class, string $method): bool {}

/** @param object|string $object_or_class */
function property_exists($object_or_class, string $property_name): bool {}

function class_exists(string $class_name, bool $autoload = true): bool {}

function interface_exists(string $interface_name, bool $autoload = true): bool {}

function trait_exists(string $trait_name, bool $autoload = true): bool {}

function function_exists(string $function_name): bool {}

function class_alias(string $original_class_name, string $alias, bool $autoload = true): bool {}

function get_included_files(): array {}

/** @alias get_included_files */
function get_required_files(): array {}

function trigger_error(string $message, int $error_type = E_USER_NOTICE): bool {}

/** @alias trigger_error */
function user_error(string $message, int $error_type = E_USER_NOTICE): bool {}

/** @return string|array|object|null */
function set_error_handler(?callable $error_handler, int $error_types = E_ALL) {}

function restore_error_handler(): bool {}

/** @return string|array|object|null */
function set_exception_handler(?callable $exception_handler) {}

function restore_exception_handler(): bool {}

function get_declared_classes(): array {}

function get_declared_traits(): array {}

function get_declared_interfaces(): array {}

function get_defined_functions(bool $exclude_disabled = true): array {}

function get_defined_vars(): array {}

/** @param resource $resource */
function get_resource_type($resource): string {}

/** @param resource $resource */
function get_resource_id($resource): int {}

function get_resources(?string $type = null): array {}

function get_loaded_extensions(bool $zend_extensions = false): array {}

function get_defined_constants(bool $categorize = false): array {}

function debug_backtrace(int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT, int $limit = 0): array {}

function debug_print_backtrace(int $options = 0, int $limit = 0): void {}

function extension_loaded(string $extension): bool {}

function get_extension_funcs(string $extension): array|false {}

#if ZEND_DEBUG && defined(ZTS)
function zend_thread_id(): int {}
#endif

function gc_mem_caches(): int {}

function gc_collect_cycles(): int {}

function gc_enabled(): bool {}

function gc_enable(): void {}

function gc_disable(): void {}

function gc_status(): array {}
