<?php

/** @generate-function-entries */

function class_implements($what, bool $autoload = true): array|false {}

function class_parents($instance, bool $autoload = true): array|false {}

function class_uses($what, bool $autoload = true): array|false {}

function spl_autoload(string $class_name, ?string $file_extensions = null): void {}

// This silently ignores non-string class names
function spl_autoload_call($class_name): void {}

function spl_autoload_extensions(?string $file_extensions = null): string {}

function spl_autoload_functions(): array|false {}

function spl_autoload_register($autoload_function = null, bool $throw = true, bool $prepend = false): bool {}

function spl_autoload_unregister($autoload_function): bool {}

function spl_classes(): array {}

function spl_object_hash(object $obj): string {}

function spl_object_id(object $obj): int {}

function iterator_apply(Traversable $iterator, callable $function, ?array $args = null): int {}

function iterator_count(Traversable $iterator): int {}

function iterator_to_array(Traversable $iterator, bool $use_keys = true): array {}
