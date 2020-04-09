<?php

/** @generate-function-entries */

function variant_set(variant $variant, $value): void {}

function variant_add($left, $right): variant {}

function variant_cat($left, $right): variant {}

function variant_sub($left, $right): variant {}

function variant_mul($left, $right): variant {}

function variant_and($left, $right): variant {}

function variant_div($left, $right): variant {}

function variant_eqv($left, $right): variant {}

function variant_idiv($left, $right): variant {}

function variant_imp($left, $right): variant {}

function variant_mod($left, $right): variant {}

function variant_or($left, $right): variant {}

function variant_pow($left, $right): variant {}

function variant_xor($left, $right): variant {}

function variant_abs($left): variant {}

function variant_fix($left): variant {}

function variant_int($left): variant {}

function variant_neg($left): variant {}

function variant_not($left): variant {}

function variant_round($left, int $decimals): ?variant {}

function variant_cmp($left, $right, int $lcid = UNKNOWN, int $flags = 0): int {}

function variant_date_to_timestamp(variant $variant): ?int {}

/** @return variant|false */
function variant_date_from_timestamp(int $timestamp) {}

function variant_get_type(variant $variant): int {}

function variant_set_type(variant $variant, int $type): void {}

function variant_cast(variant $variant, int $type): variant {}

function com_get_active_object(string $progid, int $code_page = UNKNOWN): variant {}

function com_create_guid(): string|false {}

function com_event_sink(variant $comobject, object $sinkobject, $sinkinterface = UNKNOWN): bool {}

function com_print_typeinfo($comobject, ?string $dispinterface = null, bool $wantsink = false): bool {}

function com_message_pump(int $timeoutms = 0): bool {}

function com_load_typelib(string $typelib_name, bool $case_insensitive = true): bool {}

class variant
{
    public function __construct($value = null, int $type = VT_EMPTY, int $codepage = CP_ACP) {}
}

class com
{
    /** @param string|array|null $server_name */
    public function __construct(string $module_name, $server_name = UNKNOWN, int $codepage = CP_ACP, string $typelib = "") {}
}

#if HAVE_MSCOREE_H
class dotnet
{
    public function __construct(string $assembly_name, string $datatype_name, int $codepage = CP_ACP) {}
}
#endif
