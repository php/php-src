<?php

/** @generate-function-entries */

final class EnchantBroker
{
}

final class EnchantDict
{
}

function enchant_broker_init(): EnchantBroker|false {}

/** @deprecated */
function enchant_broker_free(EnchantBroker $broker): bool {}

function enchant_broker_get_error(EnchantBroker $broker): string|false {}

/** @deprecated */
function enchant_broker_set_dict_path(EnchantBroker $broker, int $name, string $value): bool {}

/** @deprecated */
function enchant_broker_get_dict_path(EnchantBroker $broker, int $name): string|false {}

function enchant_broker_list_dicts(EnchantBroker $broker): array {}

function enchant_broker_request_dict(EnchantBroker $broker, string $tag): EnchantDict|false {}

function enchant_broker_request_pwl_dict(EnchantBroker $broker, string $filename): EnchantDict|false {}

/** @deprecated */
function enchant_broker_free_dict(EnchantDict $dict): bool {}

function enchant_broker_dict_exists(EnchantBroker $broker, string $tag): bool {}

function enchant_broker_set_ordering(EnchantBroker $broker, string $tag, string $ordering): bool {}

function enchant_broker_describe(EnchantBroker $broker): array {}

function enchant_dict_quick_check(EnchantDict $dict, string $word, &$suggestions = null): bool {}

function enchant_dict_check(EnchantDict $dict, string $word): bool {}

function enchant_dict_suggest(EnchantDict $dict, string $word): array {}

function enchant_dict_add(EnchantDict $dict, string $word): void {}

/**
* @alias enchant_dict_add
* @deprecated
*/
function enchant_dict_add_to_personal(EnchantDict $dict, string $word): void {}

function enchant_dict_add_to_session(EnchantDict $dict, string $word): void {}

function enchant_dict_is_added(EnchantDict $dict, string $word): bool {}

/**
* @param resource $dict
* @alias enchant_dict_is_added
* @deprecated
*/
function enchant_dict_is_in_session(EnchantDict $dict, string $word): bool {}

function enchant_dict_store_replacement(EnchantDict $dict, string $mis, string $cor): void {}

function enchant_dict_get_error(EnchantDict $dict): string|false {}

function enchant_dict_describe(EnchantDict $dict): array {}

