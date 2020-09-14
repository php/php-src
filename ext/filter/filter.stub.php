<?php

/** @generate-function-entries */

function filter_has_var(int $type, string $variable_name): bool {}

function filter_input(int $type, string $variable_name, int $filter = FILTER_DEFAULT, array|int $options = 0): mixed {}

function filter_var(mixed $variable, int $filter = FILTER_DEFAULT, array|int $options = 0): mixed {}

function filter_input_array(int $type, array|int $options = FILTER_DEFAULT, bool $add_empty = true): array|false|null {}

function filter_var_array(array $data, array|int $options = FILTER_DEFAULT, bool $add_empty = true): array|false|null {}

function filter_list(): array {}

function filter_id(string $filtername): int|false {}
