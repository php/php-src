<?php

function idn_to_ascii(string $domain, int $options = 0, int $variant = INTL_IDNA_VARIANT_UTS46, &$idna_info = null): string|false {}

function idn_to_utf8(string $domain, int $options = 0, int $variant = INTL_IDNA_VARIANT_UTS46, &$idna_info = null): string|false {}
