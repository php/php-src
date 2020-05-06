<?php

/** @generate-function-entries */

final class EnchantBroker
{
	/** @alias enchant_broker_init */
	public function __construct() {}

	/** @alias enchant_broker_get_error */
	public function getError(): string|false {}

	/** @alias enchant_broker_list_dicts */
	public function listDicts(): ?array {}

	/** @alias enchant_broker_request_dict */
	public function requestDict(string $tag): EnchantDict|false {}

	/** @alias enchant_broker_request_pwl_dict */
	public function requestPWL(string $filename): EnchantDict|false {}

	/** @alias enchant_broker_dict_exists */
	public function isDict(string $tag): bool {}

	/** @alias enchant_broker_set_ordering */
	public function setOrdering(string $tag, string $ordering): bool {}

	/** @alias enchant_broker_describe */
	public function describe(): ?array {}
}

final class EnchantDict
{
	public function __construct(EnchantBroker $broker, string $tag, ?string $pwl = null) {}

	/** @alias enchant_dict_quick_check */
	public function checkAndSuggest(string $word, &$suggestions = null): bool {}

	/** @alias enchant_dict_check */
	public function check(string $word): bool {}

	/** @alias enchant_dict_suggest */
	public function suggest(string $word): ?array {}

	/** @alias enchant_dict_add */
	public function add(string $word): void {}

	/** @alias enchant_dict_add_to_session */
	public function addToSession(string $word): void {}

	/** @alias enchant_dict_is_added */
	public function isAdded(string $word): bool {}

	/** @alias enchant_dict_store_replacement */
	public function storeReplacement(string $mis, string $cor): void {}

	/** @alias enchant_dict_get_error */
	public function getError(): string|false {}

	/** @alias enchant_dict_describe */
	public function describe(): array {}
}

function enchant_broker_init(): EnchantBroker|false {}

/**
* @deprecated
*/
function enchant_broker_free(EnchantBroker $broker): bool {}

function enchant_broker_get_error(EnchantBroker $broker): string|false {}

/**
* @deprecated
*/
function enchant_broker_set_dict_path(EnchantBroker $broker, int $name, string $value): bool {}

/**
* @deprecated
*/
function enchant_broker_get_dict_path(EnchantBroker $broker, int $name): string|false {}

function enchant_broker_list_dicts(EnchantBroker $broker): ?array {}

function enchant_broker_request_dict(EnchantBroker $broker, string $tag): EnchantDict|false {}

function enchant_broker_request_pwl_dict(EnchantBroker $broker, string $filename): EnchantDict|false {}

/**
* @deprecated
*/
function enchant_broker_free_dict(EnchantDict $dict): bool {}

function enchant_broker_dict_exists(EnchantBroker $broker, string $tag): bool {}

function enchant_broker_set_ordering(EnchantBroker $broker, string $tag, string $ordering): bool {}

function enchant_broker_describe(EnchantBroker $broker): ?array {}

function enchant_dict_quick_check(EnchantDict $dict, string $word, &$suggestions = null): bool {}

function enchant_dict_check(EnchantDict $dict, string $word): bool {}

function enchant_dict_suggest(EnchantDict $dict, string $word): ?array {}

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

