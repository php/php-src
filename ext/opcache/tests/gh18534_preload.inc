<?php

trait DummyTrait
{
    public ?string $dummyProperty2 {
        get => null;
    }
}

class DummyModel
{
    use DummyTrait;
}
