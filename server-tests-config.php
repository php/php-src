<?php
/* this file may be duplicated to provide testing for
   multiple php binaries or configurations.  It is used
   with the -c option on server-tests.php.  All these
   settings will also go into the environment for tests
   that are directly executed, so you can also set things
   like PHPRC here to force an executable to use a
   specific php.ini file. */
   
$conf = array(
/* path to the php source tree */
'TEST_PHP_SRCDIR'      =>    NULL,

/* executable that will be tested.  Not used for
   web based tests */
'TEST_PHP_EXECUTABLE'  =>    NULL,

/* php.ini to use when executing php */
'PHPRC'                =>    NULL,

/* log format */
'TEST_PHP_LOG_FORMAT'  =>    'LEODC',

/* debugging detail in output. */
'TEST_PHP_DETAILED'    =>    0,

/* error style for editors or IDE's */
'TEST_PHP_ERROR_STYLE' =>    'EMACS',

'REPORT_EXIT_STATUS'   =>    0,
'NO_PHPTEST_SUMMARY'   =>    0,

/* don't ask, and don't send results to QA if true */
'NO_INTERACTION'       =>    true,

/* base url prefixed to any requests */
'TEST_WEB_BASE_URL'    =>    NULL,

/* if set, copy phpt files into this directory,
   which should be accessable via an http server.  The
   TEST_WEB_BASE_URL setting should be the base url
   to access this path.  If this is not used,
   TEST_WEB_BASE_URL should be the base url pointing
   to TEST_PHP_SRCDIR, which should then be accessable via
   an http server.
   
   An example would be:
   TEST_WEB_BASE_URL=http://localhost/test
   TEST_BASE_PATH=/path/to/htdocs/test
*/
'TEST_BASE_PATH'   =>    NULL,

/* file extension of pages requested via http
   this allows for php to be configured to parse
   extensions other than php, usefull for multiple
   configurations under a single webserver */
'TEST_WEB_EXT'         =>    'php',

/* if true doesn't run tests, just outputs executable info */
'TEST_CONTEXT_INFO'    =>    false,

/* : or ; separated list of paths */
'TEST_PATHS'           =>    NULL
/* additional configuration items that may be set
   to provide proxy support for testes:
  timeout
  proxy_host
  proxy_port
  proxy_user
  proxy_pass
*/
);

?>