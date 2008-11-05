[IMPORTANT NOTICE]
------------------
This is an addendum to README.TESTING with additional information 
specific to server-tests.php.

server-tests.php is backward compatible with tests developed for
the original run-tests.php script.  server-tests is *not* used by
'make test'.  server-tests was developed to provide support for
testing PHP under it's primary environment, HTTP, and can run the
PHP tests under any of the SAPI modules that are direct executables, 
or are accessable via HTTP.

[New features]
----------------
* Command line interface:
  You can run 'php server-tests.php -h' to get all the possible options.
* Configuration file:
  the -c argument will allow you to use a configuration file.  This is
  handy if you are testing multiple environments and need various options
  depending on the environment.
  see server-tests-config.php for details.
* CGI Emulation:
  Will emulate a CGI environment when testing with the cgi sapi executable.
* HTTP testing:
  can be configured to run test scripts through an HTTP server running
  on localhost.  localhost is required since either the web server must
  alias a directory to the php source directory, or the test scripts
  must be copied to a directory under the web server 
  (see config options TEST_WEB_BASE_URL, TEST_BASE_PATH, and TEST_WEB_EXT)
* New sections supported for test files (see below)

When running tests over http, tests that require ini settings different that what
the web server runs under will be skipped.  Since the test harness defines a number
of ini settings by default, the web server may require special configuration to
make testing work.

[Example Usage]
----------------
Some (but not all!) examples of usage:

1. run tests from the php source directory
    php server-tests.php -p /path/to/php-cli

2. run tests using cgi emulation
    php server-tests.php -p /path/to/php-cgi

3. run tests over http, copying test files into document root
    php server-tests.php -w -u http://localhost/test -m /path/to/htdocs/test

4. run tests over http, php sources have been aliased in web server
    php server-tests.php -w -u http://localhost/test
    
5. run tests using configuration file
    php server-tests.php -c /path/to/server-tests-config.php

6. run tests using configuration file, but overriding some settings:
   (config file must be first)
    php server-tests.php -c /path/to/server-tests-config.php -w -t 3 -d /path/to/testdir

NOTE: configuration as described in README.TESTING still works.

[New Test Sections] 
----------------
In addition to the traditional test sections 
(see http://qa.php.net/write-test.php), several new sections are available 
under server-tests.

--POST--
This is not a new section, but not multipart posts are supported for testing
file uploads, or other types of POST data.

--CGI--
This section takes no value.  It merely provides a simple marker for tests
that MUST be run as CGI, even if there is no --POST-- or --GET-- sections
in the test file.

--DESCRIPTION--
Not used for anything, just a section for documenting the test

--ENV--
This section get's eval()'d to help build an environment for the 
execution of the test.  This can be used to change environment
vars that are used for CGI emulation, or simply to set env vars
for cli testing.  A full example looks like:

  --ENV--
  return <<<END
  PATH_TRANSLATED=$filename
  PATH_INFO=$scriptname
  SCRIPT_NAME=$scriptname
  END;

Some variables are made easily available for use in this section, they
include:
  $filename     full native path to file, will become PATH_TRANSLATED
  $filepath     =dirname($filename)
  $scriptname   this is what will become SCRIPT_NAME unless you override it
  $docroot      the equivelant of DOCUMENT_ROOT under Apache
  $cwd          the directory that the test is being initiated from
  $this->conf   all server-tests configuration vars
  $this->env    all environment variables that will get passed to the test


--REQUEST--
This section is also eval'd, and is similar in nature to --ENV--.  However,
this section is used to build the url used in an HTTP request.  Valid values
to set in this section would include:
  SCRIPT_NAME   The inital part of the request url
  PATH_INFO     The pathinfo part of a request url
  FRAGMENT      The fragment section of a url (after #)
  QUERY_STRING  The query part of a url (after ?)

  --REQUEST--
  return <<<END
  PATH_INFO=/path/info
  END;

--HEADERS--
This section is also eval'd.  It is used to provide additional headers sent
in an HTTP request, such as content type for multipart posts, cookies, etc.

  --HEADERS--
  return <<<END
  Content-Type=multipart/form-data; boundary=---------------------------240723202011929
  Content-Length=100
  END;

--EXPECTHEADERS--
This section can be used to define what headers are required to be
received back from a request, and is checked in addition to the
regular expect sections.  For example:

  --EXPECTHEADERS--
  Status: 404



