<?php

function iterator_to_array(Traversable $iterator, bool $use_keys = true): array {}

function iterator_count(Traversable $iterator): int {}

function iterator_apply(Traversable $iterator, callable $function, ?array $args = null): ?int {}

function class_parents(object|string $instance, bool $autoload = true): array|false {}

function class_implements(object|string $what, bool $autoload = true): array|false {}

function class_uses(object|string $what, bool $autoload = true): array|false {}

function spl_classes(): array {}

function spl_autoload_functions(): array|false {}

function spl_autoload(string $class_name, string $file_extensions = UNKNOWN): void {}

function spl_autoload_extensions(string $file_extensions = UNKNOWN): string {}

function spl_autoload_call(string $class_name): void {}

/**
 * @param callable $autoload_function
 */
function spl_autoload_register($autoload_function = UNKNOWN, bool $throw = true, bool $prepend = false): bool {}

function spl_autoload_unregister($autoload_function): bool {}

function spl_object_hash(object $obj): string {}

function spl_object_id(object $obj): int {}
