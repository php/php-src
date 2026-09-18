<?php
if (true) {
    interface ValueIdentifier {
        public function toString(): string;
    }
    value class CachedBookingId implements ValueIdentifier {
        public function __construct(public string $value) {}
        public function toString(): string { return $this->value; }
    }
}
