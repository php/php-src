<?php

/** @generate-class-entries */

/**
 * @strict-properties
 */
final class Closure
{
    private function __construct() {}

    public static function bind(
        Closure $closure,
        ?object $newThis,
        object|string|null $newScope = "static"
    ): ?Closure {}

    public function bindTo(?object $newThis, object|string|null $newScope = "static"): ?Closure {}

    public function call(object $newThis, mixed ...$args): mixed {}

    public static function fromCallable(callable $callback): Closure {}

    public static function fromConstExpr(string $class, string $id): Closure {}

    public static function getCurrent(): Closure {}

    public function __serialize(): array {}

    public function __unserialize(array $data): void {}
}
