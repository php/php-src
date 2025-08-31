<?php

/** @generate-class-entries */

/**
 * @param object|string $object_or_class
 * @return array<string, string>|false
 * @refcount 1
 */
function class_implements($object_or_class, bool $autoload = true): array|false {}

/**
 * @param object|string $object_or_class
 * @return array<string, string>|false
 * @refcount 1
 */
function class_parents($object_or_class, bool $autoload = true): array|false {}

/**
 * @param object|string $object_or_class
 * @return array<string, string>|false
 * @refcount 1
 */
function class_uses($object_or_class, bool $autoload = true): array|false {}

function spl_autoload(string $class, ?string $file_extensions = null): void {}

function spl_autoload_call(string $class): void {}

function spl_autoload_extensions(?string $file_extensions = null): string {}

function spl_autoload_functions(): array {}

function spl_autoload_register(?callable $callback = null, bool $throw = true, bool $prepend = false): bool {}

function spl_autoload_unregister(callable $callback): bool {}

/**
 * @return array<string, string>
 * @refcount 1
 */
function spl_classes(): array {}

/** @refcount 1 */
function spl_object_hash(object $object): string {}

function spl_object_id(object $object): int {}

function iterator_apply(Traversable $iterator, callable $callback, ?array $args = null): int {}

function iterator_count(iterable $iterator): int {}

function iterator_to_array(iterable $iterator, bool $preserve_keys = true): array {}
