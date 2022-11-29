<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue PHP_ENCHANT_MYSPELL
 * @deprecated
 */
const ENCHANT_MYSPELL = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_ENCHANT_ISPELL
 * @deprecated
 */
const ENCHANT_ISPELL = UNKNOWN;
#ifdef HAVE_ENCHANT_GET_VERSION
/**
 * @var string
 * @cvalue PHP_ENCHANT_GET_VERSION
 */
const LIBENCHANT_VERSION = UNKNOWN;
#endif

/**
 * @strict-properties
 * @not-serializable
 */
final class EnchantBroker
{
}

/**
 * @strict-properties
 * @not-serializable
 */
final class EnchantDictionary
{
}

function enchant_broker_init(): EnchantBroker|false {}

/** @deprecated */
function enchant_broker_free(EnchantBroker $broker): bool {}

function enchant_broker_get_error(EnchantBroker $broker): string|false {}

/** @deprecated */
function enchant_broker_set_dict_path(EnchantBroker $broker, int $type, string $path): bool {}

/** @deprecated */
function enchant_broker_get_dict_path(EnchantBroker $broker, int $type): string|false {}

/**
 * @return array<int, array>
 * @refcount 1
 */
function enchant_broker_list_dicts(EnchantBroker $broker): array {}

function enchant_broker_request_dict(EnchantBroker $broker, string $tag): EnchantDictionary|false {}

function enchant_broker_request_pwl_dict(EnchantBroker $broker, string $filename): EnchantDictionary|false {}

/** @deprecated */
function enchant_broker_free_dict(EnchantDictionary $dictionary): bool {}

function enchant_broker_dict_exists(EnchantBroker $broker, string $tag): bool {}

function enchant_broker_set_ordering(EnchantBroker $broker, string $tag, string $ordering): bool {}

/**
 * @return array<int, array>
 * @refcount 1
 */
function enchant_broker_describe(EnchantBroker $broker): array {}

/** @param array $suggestions */
function enchant_dict_quick_check(EnchantDictionary $dictionary, string $word, &$suggestions = null): bool {}

function enchant_dict_check(EnchantDictionary $dictionary, string $word): bool {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function enchant_dict_suggest(EnchantDictionary $dictionary, string $word): array {}

function enchant_dict_add(EnchantDictionary $dictionary, string $word): void {}

/**
* @alias enchant_dict_add
* @deprecated
*/
function enchant_dict_add_to_personal(EnchantDictionary $dictionary, string $word): void {}

function enchant_dict_add_to_session(EnchantDictionary $dictionary, string $word): void {}

function enchant_dict_is_added(EnchantDictionary $dictionary, string $word): bool {}

/**
* @alias enchant_dict_is_added
* @deprecated
*/
function enchant_dict_is_in_session(EnchantDictionary $dictionary, string $word): bool {}

function enchant_dict_store_replacement(EnchantDictionary $dictionary, string $misspelled, string $correct): void {}

function enchant_dict_get_error(EnchantDictionary $dictionary): string|false {}

/**
 * @return array<string, string>
 * @refcount 1
 */
function enchant_dict_describe(EnchantDictionary $dictionary): array {}
