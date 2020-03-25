<?php

/** @return resource|false */
function enchant_broker_init() {}

/** @param resource $broker */
function enchant_broker_free($broker): bool {}

/**
* @param resource $broker
* @return string|false
*/
function enchant_broker_get_error($broker) {}

/** @param resource $broker */
function enchant_broker_set_dict_path($broker, int $name, string $value): bool {}

/** @param resource $broker */
function enchant_broker_get_dict_path($broker, int $name): string|false {}

/** @param resource $broker */
function enchant_broker_list_dicts($broker): array {}

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

/** @param resource $dict */
function enchant_broker_free_dict($dict): bool {}

/** @param resource $broker */
function enchant_broker_dict_exists($broker, string $tag): bool {}

/** @param resource $broker */
function enchant_broker_set_ordering($broker, string $tag, string $ordering): bool {}

/** @param resource $broker */
function enchant_broker_describe($broker): array {}

/** @param resource $dict */
function enchant_dict_quick_check($dict, string $word, &$suggestions = UNKNOWN): bool {}

/** @param resource $dict */
function enchant_dict_check($dict, string $word): bool {}

/** @param resource $dict */
function enchant_dict_suggest($dict, string $word): ?array {}

/** @param resource $dict */
function enchant_dict_add_to_personal($dict, string $word): void {}

/** @param resource $dict */
function enchant_dict_add_to_session($dict, string $word): void {}

/** @param resource $dict */
function enchant_dict_is_in_session($dict, string $word): bool {}

/** @param resource $dict */
function enchant_dict_store_replacement($dict, string $mis, string $cor): void {}

/** @param resource $dict */
function enchant_dict_get_error($dict): string|false {}

/** @param resource $dict */
function enchant_dict_describe($dict): array {}
