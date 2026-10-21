<?php

/**
 * @generate-class-entries static
 * @undocumentable
 */

#[AllowDynamicProperties]
final class ZendTestSleepObject {
    public mixed $public = null;
    protected mixed $protected = null;
    private mixed $private = null;
    public int $typed = 0;
    public bool $wokenUp = false;

    public function __construct(mixed $public = null, mixed $protected = null, mixed $private = null) {}

    public function __sleep(): array {}

    public function __wakeup(): void {}
}

#[AllowDynamicProperties]
final class ZendTestSleepObjectWithCustomCreate {
    public mixed $value = null;

    public function __sleep(): array {}
}

final class ZendTestLegacySerializeObject {
    public mixed $data = null;
}
