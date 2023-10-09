<?php

function skipFunction($function): bool {
    if (false
        /* expect input / hang */
     || $function === 'readline'
     || $function === 'readline_read_history'
     || $function === 'readline_write_history'
        /* intentionally violate invariants */
     || $function === 'zend_create_unterminated_string'
     || $function === 'zend_test_array_return'
     || $function === 'zend_test_crash'
     || $function === 'zend_leak_bytes'
        /* mess with output */
     || (is_string($function) && str_starts_with($function, 'ob_'))
     || $function === 'output_add_rewrite_var'
     || $function === 'error_log'
        /* may spend a lot of time waiting for connection timeouts */
     || (is_string($function) && str_contains($function, 'connect'))
     || (is_string($function) && str_starts_with($function, 'snmp'))
     || (is_array($function) && get_class($function[0]) === mysqli::class
         && in_array($function[1], ['__construct', 'connect', 'real_connect']))
        /* misc */
     || $function === 'mail'
     || $function === 'mb_send_mail'
     || $function === 'pcntl_fork'
     || $function === 'pcntl_rfork'
     || $function === 'posix_kill'
     || $function === 'posix_setrlimit'
     || $function === 'sapi_windows_generate_ctrl_event'
     || $function === 'imagegrabscreen'
    ) {
        return true;
    }
    if ($function[0] instanceof SoapServer) {
        /* TODO: Uses fatal errors */
        return true;
    }

    return false;
}
