<?php

/** @generate-function-entries */

function zend_version(): string {}

function func_num_args(): int {}

function func_get_arg(int $position): mixed {}

function func_get_args(): array {}

function strlen(string $string): int {}

function strcmp(string $string1, string $string2): int {}

function strncmp(string $string1, string $string2, int $length): int {}

function strcasecmp(string $string1, string $string2): int {}

function strncasecmp(string $string1, string $string2, int $length): int {}

function error_reporting(?int $error_level = null): int {}

/** @param mixed $value */
function define(string $constant_name, $value, bool $case_insensitive = false): bool {}

function defined(string $constant_name): bool {}

function get_class(object $object = UNKNOWN): string {}

function get_called_class(): string {}

function get_parent_class(object|string $object_or_class = UNKNOWN): string|false {}

/** @param object|string $object_or_class */
function is_subclass_of(mixed $object_or_class, string $class, bool $allow_string = true): bool {}

/** @param object|string $object_or_class */
function is_a(mixed $object_or_class, string $class, bool $allow_string = false): bool {}

function get_class_vars(string $class): array {}

function get_object_vars(object $object): array {}

function get_mangled_object_vars(object $object): array {}

function get_class_methods(object|string $object_or_class): array {}

/** @param object|string $object_or_class */
function method_exists($object_or_class, string $method): bool {}

/** @param object|string $object_or_class */
function property_exists($object_or_class, string $property): bool {}

function class_exists(string $class, bool $autoload = true): bool {}

function interface_exists(string $interface, bool $autoload = true): bool {}

function trait_exists(string $trait, bool $autoload = true): bool {}

function function_exists(string $function): bool {}

function class_alias(string $class, string $alias, bool $autoload = true): bool {}

function get_included_files(): array {}

/** @alias get_included_files */
function get_required_files(): array {}

function trigger_error(string $message, int $error_level = E_USER_NOTICE): bool {}

/** @alias trigger_error */
function user_error(string $message, int $error_level = E_USER_NOTICE): bool {}

/** @return string|array|object|null */
function set_error_handler(?callable $callback, int $error_levels = E_ALL) {}

function restore_error_handler(): bool {}

/** @return string|array|object|null */
function set_exception_handler(?callable $callback) {}

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
