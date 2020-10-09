--TEST--
Magic Methods inheritance rules
--FILE--
<?php
class ValidMagicMethods {
    public function __call(string $name, array $arguments): mixed {}

    public static function __callStatic(string $name, array $arguments): mixed {}

    public function __clone(): void {}

    public function __debugInfo(): ?array {}

    public function __get(string $name): mixed {}

    public function __invoke(mixed $arguments): mixed {}

    public function __isset(string $name): bool {}

    public function __serialize(): array {}

    public function __set(string $name, mixed $value): void {}

    public static function __set_state(array $properties): object {}

    public function __sleep(): array {}

    public function __toString(): string {}

    public function __unserialize(array $data): void {}

    public function __unset(string $name): void {}

    public function __wakeup(): void {}
}

class NarrowedReturnType extends ValidMagicMethods {
    public function __call(string $name, array $arguments): string|float|null {}

    public static function __callStatic(string $name, array $arguments): ?array {}

    public function __debugInfo(): array {}

    public function __get(string $name): int|string {}

    public function __invoke(mixed $arguments): object {}
}

class WidenedArgumentType extends NarrowedReturnType {
    public function __call(string|array $name, array|string $arguments): string|float|null {}

    public static function __callStatic(string|object $name, array|object $arguments): ?array {}

    public function __get(string|array $name): int|string {}

    public function __isset(string|bool $name): bool {}

    public function __set(string|bool|float $name, mixed $value): void {}

    public static function __set_state(string|array $properties): object {}

    public function __unserialize(array|string $data): void {}

    public function __unset(string|array $name): void {}
}

echo 'No problems!';
?>
--EXPECT--
No problems!
