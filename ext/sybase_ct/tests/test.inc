<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id$ 
 */

  // Change if needed
  define('HOST',     getenv('PHP_SYBASE_HOST'));
  define('USER',     getenv('PHP_SYBASE_USER'));
  define('PASSWORD', getenv('PHP_SYBASE_PASS'));
  define('TEMPDB',   'tempdb');

  // {{{ bool sybase_msg_handler(int msgnumber, int severity, int state, int line, string text)
  //     Handles server messages
  function sybase_msg_handler($msgnumber, $severity, $state, $line, $text) {
    printf(
      "*** Caught Sybase Server Message #%d [Severity %d, state %d] at line %d\n    '%s'\n",
      $msgnumber,
      $severity,
      $state,
      $line,
      chop($text)
    );
  }
  // }}}

  // {{{ public class sybase
  class sybase {
  
    // {{{ public static bool static_handler(int msgnumber, int severity, int state, int line, string text)
    //     Handles server messages
    static function static_handler($msgnumber, $severity, $state, $line, $text) {
      return sybase_msg_handler($msgnumber, $severity, $state, $line, $text);
    }
    // }}}

    // {{{ public bool static_handler(int msgnumber, int severity, int state, int line, string text)
    //     Handles server messages
    function handler($msgnumber, $severity, $state, $line, $text) { 
      return sybase_msg_handler($msgnumber, $severity, $state, $line, $text);
    }
    // }}}
  }
  // }}}

  // {{{ void sybase_set_messagehandler_ex(string handler)
  //     Sets the sybase message handler and dumps the result
  function sybase_set_messagehandler_ex($handler) {
    var_dump(sybase_set_message_handler($handler));
  }
  
  // {{{ resource sybase_connect_ex(string charset= NULL, string appname= NULL, bool new= FALSE)
  //     Connect to the sybase server using the defines HOST, USER and PASSWORD
  function sybase_connect_ex($charset= NULL, $appname= NULL, $new= FALSE) {
    sybase_min_server_severity(11);       // Suppress "changed database context"
    if (!($db= sybase_connect(HOST, USER, PASSWORD, $charset ? $charset : 'iso_1', $appname, $new))) {
      die('Connect to '.USER.'@'.HOST.' failed (using password: '.(PASSWORD ? 'yes' : 'no').')');
    }
    return $db;
  }
  // }}}

  // {{{ void sybase_select_ex(resource dbh, string query)
  //     Returns all fetched rows from an SQL query
  function sybase_select_ex($dbh, $query) {
    printf(">>> Query: %s\n", $query);
    $h= sybase_query($query, $dbh);
    printf("<<< Return: %s\n", gettype($h));
    flush();
    if (!is_resource($h)) return $h;

    $return= array();
    while ($row= sybase_fetch_assoc($h)) {
      $return[]= $row;
    }
    return $return;
  }
  
  // {{{ mixed sybase_select_single(resource dbh, string query)
  //     Fires an SQL query and returns the first value from the first row
  function sybase_select_single($dbh, $query) {
    $a = sybase_fetch_row(sybase_query($query, $dbh));
    return array_shift($a);
  }
  // }}}
?>
