<?php

/** @generate-class-entries */

#[\AllowDynamicProperties]
class stdClass
{
}

/** @refcount 1 */
function zend_version(): string {}

function func_num_args(): int {}

function func_get_arg(int $position): mixed {}

/** @return array<int, mixed> */
function func_get_args(): array {}

function strlen(string $string): int {}

/** @compile-time-eval */
function strcmp(string $string1, string $string2): int {}

/** @compile-time-eval */
function strncmp(string $string1, string $string2, int $length): int {}

/** @compile-time-eval */
function strcasecmp(string $string1, string $string2): int {}

/** @compile-time-eval */
function strncasecmp(string $string1, string $string2, int $length): int {}

function error_reporting(?int $error_level = null): int {}

function define(string $constant_name, mixed $value, bool $case_insensitive = false): bool {}

function defined(string $constant_name): bool {}

function get_class(object $object = UNKNOWN): string {}

function get_called_class(): string {}

function get_parent_class(object|string $object_or_class = UNKNOWN): string|false {}

/** @param object|string $object_or_class */
function is_subclass_of(mixed $object_or_class, string $class, bool $allow_string = true): bool {}

/** @param object|string $object_or_class */
function is_a(mixed $object_or_class, string $class, bool $allow_string = false): bool {}

/**
 * @return array<string, mixed|ref>
 * @refcount 1
 */
function get_class_vars(string $class): array {}

function get_object_vars(object $object): array {}

function get_mangled_object_vars(object $object): array {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function get_class_methods(object|string $object_or_class): array {}

/** @param object|string $object_or_class */
function method_exists($object_or_class, string $method): bool {}

/** @param object|string $object_or_class */
function property_exists($object_or_class, string $property): bool {}

function class_exists(string $class, bool $autoload = true): bool {}

function interface_exists(string $interface, bool $autoload = true): bool {}

function trait_exists(string $trait, bool $autoload = true): bool {}

function enum_exists(string $enum, bool $autoload = true): bool {}

function function_exists(string $function): bool {}

function class_alias(string $class, string $alias, bool $autoload = true): bool {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function get_included_files(): array {}

/**
 * @return array<int, string>
 * @alias get_included_files
 */
function get_required_files(): array {}

function trigger_error(string $message, int $error_level = E_USER_NOTICE): bool {}

/** @alias trigger_error */
function user_error(string $message, int $error_level = E_USER_NOTICE): bool {}

/** @return callable|null */
function set_error_handler(?callable $callback, int $error_levels = E_ALL) {}

function restore_error_handler(): true {}

/** @return callable|null */
function set_exception_handler(?callable $callback) {}

function restore_exception_handler(): true {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function get_declared_classes(): array {}

/**
 * @return array<int, string>
 * @refcount 1
*/
function get_declared_traits(): array {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function get_declared_interfaces(): array {}

/**
 * @return array<string, array>
 * @refcount 1
 */
function get_defined_functions(bool $exclude_disabled = true): array {}

/**
 * @return array<string, mixed|ref>
 * @refcount 1
 */
function get_defined_vars(): array {}

/**
 * @param resource $resource
 * @refcount 1
 */
function get_resource_type($resource): string {}

/** @param resource $resource */
function get_resource_id($resource): int {}

function get_resources(?string $type = null): array {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function get_loaded_extensions(bool $zend_extensions = false): array {}

/**
 * @return array<string, mixed>
 * @refcount 1
 */
function get_defined_constants(bool $categorize = false): array {}

/**
 * @return array<int, array>
 * @refcount 1
 */
function debug_backtrace(int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT, int $limit = 0): array {}

function debug_print_backtrace(int $options = 0, int $limit = 0): void {}

function extension_loaded(string $extension): bool {}

/**
 * @return array<int, string>|false
 * @refcount 1
 */
function get_extension_funcs(string $extension): array|false {}

#if ZEND_DEBUG && defined(ZTS)
function zend_thread_id(): int {}
#endif

function gc_mem_caches(): int {}

function gc_collect_cycles(): int {}

function gc_enabled(): bool {}

function gc_enable(): void {}

function gc_disable(): void {}

/**
 * @return array<string, int|float|bool>
 * @refcount 1
 */
function gc_status(): array {}
