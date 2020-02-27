<?php

class IntlIterator implements Iterator
{
    public function current() {}

    public function key() {}

    public function next() {}

    public function rewind() {}

    public function valid() {}
}

function intl_get_error_code(): int {}

function intl_get_error_message(): string {}

function intl_is_failure(int $error_code): bool {}

function intl_error_name(int $error_code): string {}
