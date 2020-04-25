<?php

/** @generate-function-entries */

final class XmlRpcServer
{
}

function xmlrpc_encode($value): ?string {}

/** @return mixed */
function xmlrpc_decode(string $xml, string $encoding = "iso-8859-1") {}

/** @return mixed */
function xmlrpc_decode_request(string $xml, &$method, string $encoding = "iso-8859-1") {}

function xmlrpc_encode_request(?string $method, $params, array $output_options = UNKNOWN): ?string {}

/** @param mixed $value */
function xmlrpc_get_type($value): string {}

function xmlrpc_set_type(&$value, string $type): bool {}

function xmlrpc_is_fault(array $arg): bool {}

function xmlrpc_server_create(): XmlRpcServer {}

function xmlrpc_server_destroy(XmlRpcServer $server): bool {}

function xmlrpc_server_register_method(XmlRpcServer $server, string $method_name, $function): bool {}

/** @return mixed */
function xmlrpc_server_call_method(XmlRpcServer $server, string $xml, $user_data, array $output_options = UNKNOWN) {}

/** @return mixed */
function xmlrpc_parse_method_descriptions(string $xml) {}

function xmlrpc_server_add_introspection_data(XmlRpcServer $server, array $desc): int {}

function xmlrpc_server_register_introspection_callback(XmlRpcServer $server, $function): bool {}
