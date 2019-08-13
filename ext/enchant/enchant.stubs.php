<?php

/** @return resource|false */
function enchant_broker_init() {}

function enchant_broker_free(string $broker): bool {}

function enchant_broker_set_dict_path(string $broker, int $dict_type, string $value): bool {}

/** @return string|false */
function enchant_broker_get_dict_path(string $broker, int $dict_type) {}

/** @return resource|false */
function enchant_broker_request_dict(string $broker, string $tag) {}

/** @return resource|false */
function enchant_broker_request_pwl_dict(string $broker, string $filename) {}

function enchant_broker_free_dict(string $dict): bool {}

function enchant_broker_set_ordering(string $broker, string $tag, string $ordering): bool {}

function enchant_dict_quick_check(string $dict, string $word, array $suggestions = null): bool {}

function enchant_dict_check(string $dict, string $word): bool {}

function enchant_dict_store_replacement(string $dict, string $mis, string $cor): void {}
