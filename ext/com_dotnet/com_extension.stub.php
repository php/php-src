<?php

/** @generate-function-entries */

function variant_set(variant $variant, mixed $value): void {}

function variant_add(mixed $left, mixed $right): variant {}

function variant_cat(mixed $left, mixed $right): variant {}

function variant_sub(mixed $left, mixed $right): variant {}

function variant_mul(mixed $left, mixed $right): variant {}

function variant_and(mixed $left, mixed $right): variant {}

function variant_div(mixed $left, mixed $right): variant {}

function variant_eqv(mixed $left, mixed $right): variant {}

function variant_idiv(mixed $left, mixed $right): variant {}

function variant_imp(mixed $left, mixed $right): variant {}

function variant_mod(mixed $left, mixed $right): variant {}

function variant_or(mixed $left, mixed $right): variant {}

function variant_pow(mixed $left, mixed $right): variant {}

function variant_xor(mixed $left, mixed $right): variant {}

function variant_abs(mixed $left): variant {}

function variant_fix(mixed $left): variant {}

function variant_int(mixed $left): variant {}

function variant_neg(mixed $left): variant {}

function variant_not(mixed $left): variant {}

function variant_round(mixed $left, int $decimals): ?variant {}

function variant_cmp(mixed $left, mixed $right, int $lcid = LOCALE_SYSTEM_DEFAULT, int $flags = 0): int {}

function variant_date_to_timestamp(variant $variant): ?int {}

function variant_date_from_timestamp(int $timestamp): variant|false {}

function variant_get_type(variant $variant): int {}

function variant_set_type(variant $variant, int $type): void {}

function variant_cast(variant $variant, int $type): variant {}

function com_get_active_object(string $progid, ?int $code_page = null): variant {}

function com_create_guid(): string|false {}

/** @param array|string|null $sinkinterface */
function com_event_sink(variant $comobject, object $sinkobject, $sinkinterface = UNKNOWN): bool {}

function com_print_typeinfo($comobject, ?string $dispinterface = null, bool $wantsink = false): bool {}

function com_message_pump(int $timeoutms = 0): bool {}

function com_load_typelib(string $typelib_name, bool $case_insensitive = true): bool {}

class variant
{
    public function __construct(mixed $value = null, int $type = VT_EMPTY, int $codepage = CP_ACP) {}
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
