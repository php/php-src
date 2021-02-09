<?php

/** @generate-function-entries */

class LibXMLError
{
}

/** @param resource $context */
function libxml_set_streams_context($context): void {}

function libxml_use_internal_errors(?bool $use_errors = null): bool {}

function libxml_get_last_error(): LibXMLError|false {}

function libxml_get_errors(): array {}

function libxml_clear_errors(): void {}

/** @deprecated */
function libxml_disable_entity_loader(bool $disable = true): bool {}

function libxml_set_external_entity_loader(?callable $resolver_function): bool {}
