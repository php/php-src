<?php

/** @return resource|false */
function enchant_broker_init() {}

/** @resource $broker */
function enchant_broker_free($broker): bool {}

/** @resource $broker */
function enchant_broker_set_dict_path($broker, int $name, string $value): bool {}

/**
 * @param resource $broker
 * @return string|false
 */
function enchant_broker_get_dict_path($broker, int $name) {}

/**
 * @param resource $broker
 * @return resource|false
 */
function enchant_broker_request_dict($broker, string $tag) {}

/**
 * @param resource $broker
 * @return resource|false
 */
function enchant_broker_request_pwl_dict($broker, string $filename) {}

/** @resource $dict */
function enchant_broker_free_dict($dict): bool {}

/** @resource $broker */
function enchant_broker_set_ordering($broker, string $tag, string $ordering): bool {}

/** @resource $dict */
function enchant_dict_quick_check($dict, string $word, &$suggestions = UNKNOWN): bool {}

/** @resource $dict */
function enchant_dict_check($dict, string $word): bool {}

/** @resource $dict */
function enchant_dict_store_replacement($dict, string $mis, string $cor): void {}

