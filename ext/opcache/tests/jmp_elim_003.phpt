--TEST--
Incorrect empty basic block elimination
--EXTENSIONS--
opcache
--FILE--
<?php
class A {
    public static function test($args = array()) {
        if (!function_exists( 'stream_socket_client'))
            return false;

        $is_ssl = isset( $args['ssl'] ) && $args['ssl'];

        if ($is_ssl) {
            if (!extension_loaded( 'openssl'))
                return false;
            if (!function_exists('openssl_x509_parse'))
                return false;
        }

        return apply_filters('use_streams_transport', true, $args);
    }
}
?>
OK
--EXPECT--
OK
