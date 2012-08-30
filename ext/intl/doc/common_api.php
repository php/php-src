<?php

/**
 * Handling of errors occured in static methods
 * when there's no object to get error code/message from.
 *
 * Example #1:
 * <code>
 * $coll = collator_create( '<bad_param>' );
 * if( !$coll )
 *     handle_error( intl_get_error_code() );
 * </code>
 *
 * Example #2:
 * <code>
 * if( Collator::getAvailableLocales() === false )
 *     show_error( intl_get_error_message() );
 * </code>
 */

/**
 * Get the last error code.
 *
 * @return int     Error code returned by the last
 *                 API function call.
 */
function intl_get_error_code() {}

/**
 * Get description of the last error.
 *
 * @return string  Description of an error occured in the last
 *                 API function call.
 */
function intl_get_error_message() {}

/**
 * Check whether the given error code indicates failure.
 *
 * @param  int  $code ICU error code.
 *
 * @return bool true if it the code indicates some failure,
 *              and false in case of success or a warning.
 */
function intl_is_failure($code) {}

/**
 * Get symbolic name for a given error code.
 *
 * The returned string will be the same as the name of the error code constant.
 *
 * @param  int       $code  ICU error code.
 *
 * @return string    Error code name.
 */
function intl_error_name($code) {}
