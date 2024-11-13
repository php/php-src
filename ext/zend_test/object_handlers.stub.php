<?php

/**
 * @generate-class-entries static
 * @undocumentable
 */

final class DoOperationNoCast {
    private int $val;
    public function __construct(int $val) {}
}

final class LongCastableNoOperations {
    private int $val;
    public function __construct(int $val) {}
}
final class FloatCastableNoOperations {
    private float $val;
    public function __construct(float $val) {}
}
final class NumericCastableNoOperations {
    private int|float $val;
    public function __construct(int|float $val) {}
}

class DimensionHandlersNoArrayAccess {
    public bool $read = false;
    public bool $write = false;
    public bool $has = false;
    public bool $unset = false;
    public int $readType;
    public bool $hasOffset = false;
    public int $checkEmpty;
    public mixed $offset;
}
