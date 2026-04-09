<?php

/** @generate-class-entries */

namespace Uuid {
    final readonly class UuidV7
    {
        public static function parse(string $uuid): static {}

        public static function generate(?\DateTimeImmutable $datetime = null, ?\Random\Engine $randomEngine = null): static {}

        private function __construct() {}

        public function toBytes(): string {}

        public function toString(): string {}

        public function equals(\Uuid\UuidV7 $uuid): bool {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}

        public function __debugInfo(): array {}
    }
}
