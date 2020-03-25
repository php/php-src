<?php

function xmlrpc_encode($value): ?string {}

function xmlrpc_decode(string $xml, string $encoding = "iso-8859-1") {}

function xmlrpc_decode_request(string $xml, &$method, string $encoding = "iso-8859-1") {}

function xmlrpc_encode_request(?string $method, $params, array $output_options = UNKNOWN): ?string {}

function xmlrpc_set_type(&$value, string $type): bool {}

function xmlrpc_is_fault(array $arg): bool {}

/** @return resource */
function xmlrpc_server_create() {}

/** @param resource $server */
function xmlrpc_server_destroy($server): bool {}

/** @param resource $server */
function xmlrpc_server_register_method($server, string $method_name, $function): bool {}

/** @param resource $server */
function xmlrpc_server_call_method($server, string $xml, $user_data, array $output_options = UNKNOWN) {}

function xmlrpc_parse_method_descriptions(string $xml) {}

/** @param resource $server */
function xmlrpc_server_add_introspection_data($server, array $desc): int {}

/** @param resource $server */
function xmlrpc_server_register_introspection_callback($server, $function): bool {}
