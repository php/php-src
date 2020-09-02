<?php

/** @generate-function-entries */

final class EnchantBroker
{
}

final class EnchantDictionary
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

function enchant_broker_request_dict(EnchantBroker $broker, string $tag): EnchantDictionary|false {}

function enchant_broker_request_pwl_dict(EnchantBroker $broker, string $filename): EnchantDictionary|false {}

/** @deprecated */
function enchant_broker_free_dict(EnchantDictionary $dict): bool {}

function enchant_broker_dict_exists(EnchantBroker $broker, string $tag): bool {}

function enchant_broker_set_ordering(EnchantBroker $broker, string $tag, string $ordering): bool {}

function enchant_broker_describe(EnchantBroker $broker): array {}

/** @param array $suggestions */
function enchant_dict_quick_check(EnchantDictionary $dict, string $word, &$suggestions = null): bool {}

function enchant_dict_check(EnchantDictionary $dict, string $word): bool {}

function enchant_dict_suggest(EnchantDictionary $dict, string $word): array {}

function enchant_dict_add(EnchantDictionary $dict, string $word): void {}

/**
* @alias enchant_dict_add
* @deprecated
*/
function enchant_dict_add_to_personal(EnchantDictionary $dict, string $word): void {}

function enchant_dict_add_to_session(EnchantDictionary $dict, string $word): void {}

function enchant_dict_is_added(EnchantDictionary $dict, string $word): bool {}

/**
* @alias enchant_dict_is_added
* @deprecated
*/
function enchant_dict_is_in_session(EnchantDictionary $dict, string $word): bool {}

function enchant_dict_store_replacement(EnchantDictionary $dict, string $mis, string $cor): void {}

function enchant_dict_get_error(EnchantDictionary $dict): string|false {}

function enchant_dict_describe(EnchantDictionary $dict): array {}
