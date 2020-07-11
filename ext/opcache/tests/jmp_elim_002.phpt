--TEST--
Incorrect empty basic block elimination
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function wp_get_archives( $args = '' ) {
    $defaults = array(
        'type' => 'monthly', 'limit' => '',
        'format' => 'html', 'before' => '',
        'after' => '', 'show_post_count' => false,
        'echo' => 1, 'order' => 'DESC',
    );

    $r = wp_parse_args( $args, $defaults );

    if ( ! empty( $r['limit'] ) ) {
        $r['limit'] = absint( $r['limit'] );
        $r['limit'] = ' LIMIT ' . $r['limit'];
    }

    $archive_date_format_over_ride = 0;
    $archive_day_date_format = 'Y/m/d';

    if (!$archive_date_format_over_ride ) {
        $archive_day_date_format = get_option( 'date_format' );
    }

    if ( $r['echo'] ) {
        echo $output;
    } else {
        return $output;
    }
}
?>
OK
--EXPECT--
OK
