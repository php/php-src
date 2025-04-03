<?php

function

class GenericTestClass<T> {
    // T can be used as type alias inside of 'GenericTestClass'
    public function __construct(
        private T $property
    ) {}

    public function getMyT(): T {
        return $this->property;
    }

    public function setMyT(T $newValue) {
        $this->property = $newValue;
    }
}