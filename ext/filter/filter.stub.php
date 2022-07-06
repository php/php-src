<?php

/** @generate-function-entries */

function filter_has_var(int $input_type, string $var_name): bool {}

function filter_input(int $type, string $var_name, int $filter = FILTER_DEFAULT, array|int $options = 0): mixed {}

function filter_var(mixed $value, int $filter = FILTER_DEFAULT, array|int $options = 0): mixed {}

function filter_input_array(int $type, array|int $options = FILTER_DEFAULT, bool $add_empty = true): array|false|null {}

function filter_var_array(array $array, array|int $options = FILTER_DEFAULT, bool $add_empty = true): array|false|null {}

function filter_list(): array {}

function filter_id(string $name): int|false {}
