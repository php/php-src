# Sections

A phpt test can have many more parts than just the minimum. In fact some of the mandatory parts have
alternatives that may be used if the situation warrants it. The phpt sections are documented here.

## `--TEST--`

**Description:** Title of test as a single line short description.

**Required:** Yes

**Format:** Plain text. We recommend a single line only.

Example 1 (snippet):

```text

   --TEST--
   Test filter_input() with GET and POST data.
```

Example 1 (full): {ref}`sample001.phpt`

## `--DESCRIPTION--`

**Description:** If your test requires more than a single line title to adequately describe it, you
can use this section for further explanation. Multiple lines are allowed and besides being used for
information, this section is completely ignored by the test binary.

**Required:** No

**Format:** Plain text, multiple lines.

Example 1 (snippet):

```text

   --DESCRIPTION--
   This test covers both valid and invalid usages of filter_input() with INPUT_GET and INPUT_POST data and several different filter sanitizers.
```

Example 1 (full): {ref}`sample001.phpt`

## `--CREDITS--`

**Description:** Used to credit contributors without CVS commit rights, who put their name and email
on the first line. If the test was part of a TestFest event, then # followed by the name of the
event and the date (YYYY-MM-DD) on the second line.

**Required:** No. For newly created tests the section should no longer be used for simple authorship
claims or listing all contributors who edited the test; as it is already accurately tracked by Git.
It may be used if more specific attribution is useful, for example to credit the original reporter
of a bug or a contributor who is not credited via `Co-authored-by` tag.

**Format:** Name Email [Event]

Example 1 (snippet):

```text

   --CREDITS--
   Felipe Pena
```

Example 1 (full): {ref}`sample001.phpt`

Example 2 (snippet):

```text

   --CREDITS--
   Zoe Slattery zoe@php.net
   # TestFest Munich 2009-05-19
```

Example 2 (full): {ref}`sample002.phpt`

## `--SKIPIF--`

**Description:** A condition or set of conditions used to determine if a test should be skipped.
Tests that are only applicable to a certain platform, extension or PHP version are good reasons for
using a `--SKIPIF--` section.

A common practice for extension tests is to write your `--SKIPIF--` extension criteria into a file
call skipif.inc and then including that file in the `--SKIPIF--` section of all your extension
tests. This promotes the DRY principle and reduces future code maintenance.

**Required:** No.

**Format:** PHP code enclosed by PHP tags. If the output of this scripts starts with "skip", the
test is skipped. If the output starts with "xfail", the test is marked as expected failure. If the
output starts with "flaky", the test is marked as flaky test. The "xfail" convention is supported as
of PHP 7.2.0. The "flaky" convention is supported as of PHP 8.2.25 and PHP 8.3.13, respectively.

Example 1 (snippet):

```php

   --SKIPIF--
   <?php if (!extension_loaded("filter")) die("Skipped: filter extension required."); ?>
```

Example 1 (full): {ref}`sample001.phpt`

Example 2 (snippet):

```php

   --SKIPIF--
   <?php include('skipif.inc'); ?>
```

Example 2 (full): {ref}`sample003.phpt`

Example 3 (snippet):

```php

   --SKIPIF--
   <?php if (getenv('SKIP_ASAN')) die('xfail Startup failure leak'); ?>
```

Example 3 (full): {ref}`xfailif.phpt`

Example 4 (snippet):

```php

   --SKIPIF--
   <?php
   if (getenv("GITHUB_ACTIONS") && PHP_OS_FAMILY === "Darwin") {
           die("flaky Occasionally segfaults on macOS for unknown reasons");
   }
```

Some tests depend on modules or functions available only in certain versions or they even require
minimum version of php or zend. These tests should be skipped when the requirement cannot be
fulfilled. To achieve this you can use the `SKIPIF` section. To tell `run-tests.php` that your
test should be skipped the `SKIPIF` section must print out the word "skip" followed by a reason
why the test should skip.

*ext/sodium/tests/pwhash_argon2i.phpt*

```php

   --TEST--
   Check for libsodium argon2i
   --EXTENSIONS--
   sodium
   --SKIPIF--
   <?php
   if (!defined('SODIUM_CRYPTO_PWHASH_SALTBYTES')) print "skip libsodium without argon2i";
   ?>
   --FILE--
   [snip]
```

Test script and `SKIPIF` code should be directly written into `\*.phpt`. However, it is
recommended to use include files when more test scripts depend on the same `SKIPIF` code or when
certain test files need the same values for some input.

> [!NOTE]
> No file used by any test should have one of the following extensions: ".php", ".log", ".mem",
> ".exp", ".out" or ".diff". When you use an include file for the `SKIPIF` section it should be
> named "skipif.inc" and an include file used in the `FILE` section of many tests should be named
> "test.inc".

## `--CONFLICTS--`

**Description:** This section is only relevant for parallel test execution (available as of PHP
7.4.0), and allows to specify conflict keys. While a test that conflicts with key K is running, no
other test that conflicts with K is run. For tests conflicting with "all", no other tests are run in
parallel.

An alternative to have a `--CONFLICTS--` section is to add a file named `CONFLICTS` to the
directory containing the tests. The contents of the `CONFLICTS` file must have the same format as
the contents of the `--CONFLICTS--` section.

**Required:** No.

**Format:** One conflict key per line. Comment lines starting with # are also allowed.

Example 1 (snippet):

```text

   --CONFLICTS--
   server
```

Example 1 (full): {ref}`conflicts_1.phpt`

## `--WHITESPACE_SENSITIVE--`

**Description:** This flag is used to indicate that the test should not be changed by automated
formatting changes. Available as of PHP 7.4.3.

**Required:** No.

**Format:** No value, just the `--WHITESPACE_SENSITIVE--` statement.

## `--CAPTURE_STDIO--`

**Description:** This section enables which I/O streams the `run-tests.php` test script will use
when comparing executed file to the expected output. The `STDIN` is the standard input stream.
When `STDOUT` is enabled, the test script will also check the contents of the standard output.
`When STDERR` is enabled, the test script will also compare the contents of the standard error I/O
stream.

If this section is left out of the test, by default, all three streams are enabled, so the tests
without this section capture all and is the same as enabling all three manually.

**Required:** No.

**Format:** A case insensitive space, newline or otherwise delimited list of one or more strings of
STDIN, STDOUT, and/or STDERR.

Example 1 (snippet):

```text

   --CAPTURE_STDIO--
   STDIN STDERR
```

Example 1 (full): {ref}`capture_stdio_1.phpt`

Example 2 (snippet):

```text

   --CAPTURE_STDIO--
   STDIN STDOUT
```

Example 2 (full): {ref}`capture_stdio_2.phpt`

Example 3 (snippet):

```text

   --CAPTURE_STDIO--
   STDIN STDOUT STDERR
```

Example 3(full): {ref}`capture_stdio_3.phpt`

## `--EXTENSIONS--`

**Description:** Additional required shared extensions to be loaded when running the test. When the
`run-tests.php` script is executed it loads all the extensions that are available and enabled for
that particular PHP at the time. If the test requires additional extension to be loaded and they
aren't loaded prior to running the test, this section loads them.

**Required:** No.

**Format:** A case sensitive newline separated list of extension names.

Example 1 (snippet):

```text

   --EXTENSIONS--
   curl
   imagick
   tokenizer
```

Example 1 (full): {ref}`extensions.phpt`

Some tests depend on PHP extensions that may be unavailable. These extensions should be listed in
the `EXTENSIONS` section. If an extension is missing, PHP will try to find it in a shared module
and skip the test if it's not there.

*/ext/sodium/tests/crypto_scalarmult.phpt*

```php

   --TEST--
   Check for libsodium scalarmult
   --EXTENSIONS--
   sodium
   --FILE--
   <?php
   $n = sodium_hex2bin("5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb");
```

## `--POST--`

**Description:** POST variables or data to be passed to the test script. This section forces the use
of the CGI binary instead of the usual CLI one.

**Required:** No.

Requirements: PHP CGI binary.

**Format:** Follows the HTTP post data format.

Example 1 (snippet):

```text

   --POST--
   c=<p>string</p>&d=12345.7
```

Example 1 (full): {ref}`sample001.phpt`

Example 2 (snippet):

```xml

   --POST--
   <SOAP-ENV:Envelope
     SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
     xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
     xmlns:xsd="http://www.w3.org/2001/XMLSchema"
     xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xmlns:si="http://soapinterop.org/xsd">
     <SOAP-ENV:Body>
           <ns1:test xmlns:ns1="http://testuri.org" />
     </SOAP-ENV:Body>
   </SOAP-ENV:Envelope>
```

Example 2 (full): {ref}`sample005.phpt`

## `--POST_RAW--`

**Description:** Raw POST data to be passed to the test script. This differs from the section above
because it doesn't automatically set the Content-Type, this leaves you free to define your own
within the section. This section forces the use of the CGI binary instead of the usual CLI one.

**Required:** No.

Requirements: PHP CGI binary.

**Test Script Support:** `run-tests.php`

**Format:** Follows the HTTP post data format.

Example 1 (snippet):

```text

   --POST_RAW--
   Content-type: multipart/form-data, boundary=AaB03x

   --AaB03x content-disposition: form-data; name="field1"

   Joe Blow
   --AaB03x
   content-disposition: form-data; name="pics"; filename="file1.txt"
   Content-Type: text/plain

   abcdef123456789
   --AaB03x--
```

Example 1 (full): {ref}`sample006.phpt`

## `--PUT--`

**Description:** Similar to the section above, PUT data to be passed to the test script. This
section forces the use of the CGI binary instead of the usual CLI one.

**Required:** No.

Requirements: PHP CGI binary.

**Test Script Support:** `run-tests.php`

**Format:** Raw data optionally preceded by a Content-Type header.

Example 1 (snippet):

```text

   --PUT--
   Content-Type: text/json

   {"name":"default output handler","type":0,"flags":112,"level":0,"chunk_size":0,"buffer_size":16384,"buffer_used":3}
```

## `--GZIP_POST--`

**Description:** When this section exists, the POST data will be gzencode()'d. This section forces
the use of the CGI binary instead of the usual CLI one.

**Required:** No.

**Test Script Support:** `run-tests.php`

**Format:** Just add the content to be gzencode()'d in the section.

Example 1 (snippet):

```xml

   --GZIP_POST--
   <SOAP-ENV:Envelope
     SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
     xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
     xmlns:xsd="http://www.w3.org/2001/XMLSchema"
     xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xmlns:si="http://soapinterop.org/xsd">
     <SOAP-ENV:Body>
           <ns1:test xmlns:ns1="http://testuri.org" />
     </SOAP-ENV:Body>
   </SOAP-ENV:Envelope>
```

Example 1 (full): {ref}`sample005.phpt`

## `--DEFLATE_POST--`

**Description:** When this section exists, the POST data will be gzcompress()'ed. This section
forces the use of the CGI binary instead of the usual CLI one.

**Required:** No.

Requirements:

**Test Script Support:** `run-tests.php`

**Format:** Just add the content to be gzcompress()'ed in the section.

Example 1 (snippet):

```xml

   --DEFLATE_POST--
   <?xml version="1.0" encoding="ISO-8859-1"?>
   <SOAP-ENV:Envelope
     SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
     xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
     xmlns:xsd="http://www.w3.org/2001/XMLSchema"
     xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xmlns:si="http://soapinterop.org/xsd">
     <SOAP-ENV:Body>
           <ns1:test xmlns:ns1="http://testuri.org" />
     </SOAP-ENV:Body>
   </SOAP-ENV:Envelope>
```

Example 1 (full): {ref}`sample007.phpt`

## `--GET--`

**Description:** GET variables to be passed to the test script. This section forces the use of the
CGI binary instead of the usual CLI one.

**Required:** No.

Requirements: PHP CGI binary.

**Format:** A single line of text passed as the GET data to the script.

Example 1 (snippet):

```text

   --GET--
   a=<b>test</b>&b=http://example.com
```

Example 1 (full): {ref}`sample001.phpt`

Example 2 (snippet):

```text

   --GET--
   ar[elm1]=1234&ar[elm2]=0660&a=0234
```

Example 2 (full): {ref}`sample008.phpt`

## `--COOKIE--`

**Description:** Cookies to be passed to the test script. This section forces the use of the CGI
binary instead of the usual CLI one.

**Required:** No.

Requirements: PHP CGI binary.

**Test Script Support:** `run-tests.php`

**Format:** A single line of text in a valid HTTP cookie format.

Example 1 (snippet):

```

   --COOKIE--
   hello=World;goodbye=MrChips
```

Example 1 (full): {ref}`sample002.phpt`

## `--STDIN--`

**Description:** Data to be fed to the test script's standard input.

**Required:** No.

**Test Script Support:** `run-tests.php`

**Format:** Any text within this section is passed as STDIN to PHP.

Example 1 (snippet):

```text

   --STDIN--
   fooBar
   use this to input some thing to the php script
```

Example 1 (full): {ref}`sample009.phpt`

## `--INI--`

**Description:** To be used if you need a specific php.ini setting for the test.

**Required:** No.

**Format:** Key value pairs including automatically replaced tags. One setting per line. Content
that is not a valid ini setting may cause failures.

The following is a list of all tags and what they are used to represent:

-  `{PWD}`: Represents the directory of the file containing the `--INI--` section.
-  `{TMP}`: Represents the system's temporary directory. Available as of PHP 7.2.19 and 7.3.6.

Example 1 (snippet):

```text

   --INI--
   precision=14
```

Example 1 (full): {ref}`sample001.phpt`

Example 2 (snippet):

```text

   --INI--
   session.use_cookies=0
   session.cache_limiter=
   register_globals=1
   session.serialize_handler=php
   session.save_handler=files
```

Example 2 (full): {ref}`sample003.phpt`

## `--ARGS--`

**Description:** A single line defining the arguments passed to PHP.

**Required:** No.

**Format:** A single line of text that is passed as the argument(s) to the PHP CLI.

Example 1 (snippet):

```text

   --ARGS--
   --arg value --arg=value -avalue -a=value -a value
```

Example 1 (full): {ref}`sample010.phpt`

## `--ENV--`

**Description:** Configures environment variables such as those found in the `$_SERVER` global
array.

**Required:** No.

**Format:** Key value pairs. One setting per line.

Example 1 (snippet):

```text

   --ENV--
   SCRIPT_NAME=/frontcontroller10.php
   REQUEST_URI=/frontcontroller10.php/hi
   PATH_INFO=/hi
```

Example 1 (full): {ref}`sample018.phpt`

## `--PHPDBG--`

**Description:** This section takes arbitrary phpdbg commands and executes the test file according
to them as it would be run in the phpdbg prompt.

**Required:** No.

**Format:** Arbitrary phpdbg commands

Example 1 (snippet):

```text

   --PHPDBG--
   b
   4
   b
   del
   0
   b
   5
   r
   b
   del
   1
   r
   y
   q
```

Example 1 (full): {ref}`phpdbg_1.phpt`

## `--FILE--`

**Description:** The test source code.

**Required:** One of the `FILE` type sections is required.

**Format:** PHP source code enclosed by PHP tags.

Example 1 (snippet):

```php

   --FILE--
   <?php
   ini_set('html_errors', false);
   var_dump(filter_input(INPUT_GET, "a", FILTER_SANITIZE_STRIPPED));
   var_dump(filter_input(INPUT_GET, "b", FILTER_SANITIZE_URL));
   var_dump(filter_input(INPUT_GET, "a", FILTER_SANITIZE_SPECIAL_CHARS, array(1,2,3,4,5)));
   var_dump(filter_input(INPUT_GET, "b", FILTER_VALIDATE_FLOAT, new stdClass));
   var_dump(filter_input(INPUT_POST, "c", FILTER_SANITIZE_STRIPPED, array(5,6,7,8)));
   var_dump(filter_input(INPUT_POST, "d", FILTER_VALIDATE_FLOAT));
   var_dump(filter_input(INPUT_POST, "c", FILTER_SANITIZE_SPECIAL_CHARS));
   var_dump(filter_input(INPUT_POST, "d", FILTER_VALIDATE_INT));
   var_dump(filter_var(new stdClass, "d"));
   var_dump(filter_input(INPUT_POST, "c", "", ""));
   var_dump(filter_var("", "", "", "", ""));
   var_dump(filter_var(0, 0, 0, 0, 0));
   echo "Done\n";
   ?>
```

Example 1 (full): {ref}`sample001.phpt`

## `--FILEEOF--`

**Description:** An alternative to `--FILE--` where any trailing line breaks (n || r || rn
found at the end of the section) are omitted. This is an extreme edge-case feature, so 99.99% of the
time you won't need this section.

**Required:** One of the `FILE` type sections is required.

**Test Script Support:** `run-tests.php`

**Format:** PHP source code enclosed by PHP tags.

Example 1 (snippet):

```php

   --FILEEOF--
   <?php
   eval("echo 'Hello'; // comment");
   echo " World";
   //last line comment
```

Example 1 (full): {ref}`sample011.phpt`

## `--FILE_EXTERNAL--`

**Description:** An alternative to `--FILE--`. This is used to specify that an external file
should be used as the `--FILE--` contents of the test file, and is designed for running the same
test file with different ini, environment, post/get or other external inputs. Basically it allows
you to DRY up some of your tests. The file must be in the same directory as the test file, or in a
subdirectory.

**Required:** One of the `FILE` type sections is required.

**Test Script Support:** `run-tests.php`

**Format:** path/to/file. Single line.

Example 1 (snippet):

```text

   --FILE_EXTERNAL--
   files/file012.inc
```

Example 1 (full): {ref}`sample012.phpt`

## `--REDIRECTTEST--`

**Description:** This block allows you to redirect from one test to a bunch of other tests. It also
allows you to set configurations which are used on all tests in your destination.

**Required:** One of the `FILE` type sections is required.

**Test Script Support:** `run-tests.php`

**Format:** PHP source which is run through `eval()`. The tests destination is the value of an
array index 'TESTS'. Also, keep in mind, you can not use a `REDIRECTTEST` which is being pointed
to by another test which contains a `REDIRECTTEST`. In other words, no nesting.

The relative path declared in 'TESTS' is relative to the base directory for the PHP source code, not
relative to the current directory.

Last note, the array in this section must be returned to work.

Example 1 (snippet):

```php

   --REDIRECTTEST--
   return array(
     'ENV' => array(
             'PDOTEST_DSN' => 'sqlite2::memory:'
           ),
     'TESTS' => 'ext/pdo/tests'
     );
```

Example 1 (full): {ref}`sample013.phpt`

> [!NOTE]
> The destination tests for this example are not included. See the PDO extension tests for
> reference to live tests using this section.

Example 2 (snippet):

```php

   --REDIRECTTEST--
   # magic auto-configuration

   $config = array(
     'TESTS' => 'ext/pdo/tests'
   );

   if (false !== getenv('PDO_MYSQL_TEST_DSN')) {
     # user set them from their shell
     $config['ENV']['PDOTEST_DSN'] = getenv('PDO_MYSQL_TEST_DSN');
     $config['ENV']['PDOTEST_USER'] = getenv('PDO_MYSQL_TEST_USER');
     $config['ENV']['PDOTEST_PASS'] = getenv('PDO_MYSQL_TEST_PASS');
     if (false !== getenv('PDO_MYSQL_TEST_ATTR')) {
           $config['ENV']['PDOTEST_ATTR'] = getenv('PDO_MYSQL_TEST_ATTR');
     }
   } else {
     $config['ENV']['PDOTEST_DSN'] = 'mysql:host=localhost;dbname=test';
     $config['ENV']['PDOTEST_USER'] = 'root';
     $config['ENV']['PDOTEST_PASS'] = '';
   }

   return $config;
```

Example 2 (full): {ref}`sample014.phpt`

> [!NOTE]
> The destination tests for this example are not included. See the PDO extension tests for
> reference to live tests using this section.

## `--CGI--`

**Description:** This section takes no value. It merely provides a simple marker for tests that MUST
be run as CGI, even if there is no `--POST--` or `--GET--` sections in the test file.

**Required:** No.

**Format:** No value, just the `--CGI--` statement.

Example 1 (snippet):

```text

   --CGI--
```

Example 1 (full): {ref}`sample016.phpt`

## `--XFAIL--`

**Description:** This section identifies this test as one that is currently expected to fail. It
should include a brief description of why it's expected to fail. Reasons for such expectations
include tests that are written before the functionality they are testing is implemented or notice of
a bug which is due to upstream code such as an extension which provides PHP support for some other
software.

Please do NOT include an `--XFAIL--` without providing a text description for the reason it's
being used.

**Required:** No.

**Test Script Support:** `run-tests.php`

**Format:** A short plain text description of why this test is currently expected to fail.

Example 1 (snippet):

```text

   --XFAIL--
   This bug might be still open on aix5.2-ppc64 and hpux11.23-ia64
```

Example 1 (full): {ref}`sample017.phpt`

## `--FLAKY--`

**Description:** This section identifies this test as one that occasionally fails. If the test
actually fails, it will be retried one more time, and that result will be reported. The section
should include a brief description of why the test is flaky. Reasons for this include tests that
rely on relatively precise timing, or temporary disc states. Available as of PHP 8.1.22 and 8.2.9,
respectively.

Please do NOT include a `--FLAKY--` section without providing a text description for the reason it
is being used.

**Required:** No.

**Test Script Support:** `run-tests.php`

**Format:** A short plain text description of why this test is flaky.

Example 1 (snippet):

```

   --FLAKY--
   This test frequently fails in CI
```

Example 1 (full): flaky.phpt

## `--EXPECTHEADERS--`

**Description:** The expected headers. Any header specified here must exist in the response and have
the same value or the test fails. Additional headers found in the actual tests while running are
ignored.

**Required:** No.

**Format:** HTTP style headers. May include multiple lines.

Example 1 (snippet):

--EXPECTHEADERS-- Status: 404

Example 1 (snippet):

```text

   --EXPECTHEADERS--
   Content-type: text/html; charset=UTF-8
   Status: 403 Access Denied
```

Example 1 (full): {ref}`sample018.phpt`

> [!NOTE]
> The destination tests for this example are not included. See the phar extension tests for
> reference to live tests using this section.

## `--EXPECT--`

**Description:** The expected output from the test script. This must match the actual output from
the test script exactly for the test to pass.

**Required:** One of the `EXPECT` type sections is required.

**Format:** Plain text. Multiple lines of text are allowed.

Example 1 (snippet):

```text

   --EXPECT--
   array(2) {
     ["hello"]=>
     string(5) "World"
     ["goodbye"]=>
     string(7) "MrChips"
   }
```

Example 1 (full): {ref}`sample002.phpt`

## `--EXPECT_EXTERNAL--`

**Description:** Similar to `--EXPECT--` section, but just stating a filename where to load the
expected output from.

**Required:** One of the `EXPECT` type sections is required.

**Test Script Support:** `run-tests.php`

Example 1 (snippet):

```text

   --EXPECT_EXTERNAL--
   test001.expected.txt
```

*test001.expected.txt*

```php

   array(2) {
     ["hello"]=>
     string(5) "World"
     ["goodbye"]=>
     string(7) "MrChips"
   }
```

## `--EXPECTF--`

**Description:** An alternative of `--EXPECT--`. Where it differs from `--EXPECT--` is that it
uses a number of substitution tags for strings, spaces, digits, etc. that appear in test case output
but which may vary between test runs. The most common example of this is to use %s and %d to match
the file path and line number which are output by PHP Warnings.

**Required:** One of the `EXPECT` type sections is required.

**Format:** Plain text including tags which are inserted to represent different types of output
which are not guaranteed to have the same value on subsequent runs or when run on different
platforms.

The following is a list of all tags and what they are used to represent:

> - `%e`: Represents a directory separator, for example / on Linux.
> - `%s`: One or more of anything (character or white space) except the end of line character.
> - `%S`: Zero or more of anything (character or white space) except the end of line character.
> - `%a`: One or more of anything (character or white space) including the end of line
>   character.
> - `%A`: Zero or more of anything (character or white space) including the end of line
>   character.
> - `%w`: Zero or more white space characters.
> - `%i`: A signed integer value, for example +3142, -3142, 3142.
> - `%d`: An unsigned integer value, for example 123456.
> - `%x`: One or more hexadecimal character. That is, characters in the range 0-9, a-f, A-F.
> - `%f`: A floating point number, for example: 3.142, -3.142, 3.142E-10, 3.142e+10.
> - `%c`: A single character of any sort (.).
> - `%r...%r`: Any string (...) enclosed between two `%r` will be treated as a regular
>   expression.

Example 1 (snippet):

```text

   --EXPECTF--
   string(4) "test"
   string(18) "http://example.com"
   string(27) "&#60;b&#62;test&#60;/b&#62;"

   Notice: Object of class stdClass could not be converted to int in %ssample001.php on line %d
   bool(false)
   string(6) "string"
   float(12345.7)
   string(29) "&#60;p&#62;string&#60;/p&#62;"
   bool(false)

   Warning: filter_var() expects parameter 2 to be long, string given in %s011.php on line %d
   NULL

   Warning: filter_input() expects parameter 3 to be long, string given in %s011.php on line %d
   NULL

   Warning: filter_var() expects at most 3 parameters, 5 given in %s011.php on line %d
   NULL

   Warning: filter_var() expects at most 3 parameters, 5 given in %s011.php on line %d
   NULL
   Done
```

Example 1 (full): {ref}`sample001.phpt`

Example 2 (snippet):

```text

   --EXPECTF--
   Warning: bzopen() expects exactly 2 parameters, 0 given in %s on line %d NULL

   Warning: bzopen(): '' is not a valid mode for bzopen(). Only 'w' and 'r' are supported. in %s on line %d
   bool(false)

   Warning: bzopen(): filename cannot be empty in %s on line %d
   bool(false)

   Warning: bzopen(): filename cannot be empty in %s on line %d
   bool(false)

   Warning: bzopen(): 'x' is not a valid mode for bzopen(). Only 'w' and 'r' are supported. in %s on line %d
   bool(false)

   Warning: bzopen(): 'rw' is not a valid mode for bzopen(). Only 'w' and 'r' are supported. in %s on line %d
   bool(false)

   Warning: bzopen(no_such_file): failed to open stream: No such file or directory in %s on line %d
   bool(false)
   resource(%d) of type (stream) Done
```

Example 2 (full): {ref}`sample019.phpt`

Example 3 (snippet):

```text

   --EXPECTF--
   object(DOMNodeList)#%d (0) {
   }
   int(0)
   bool(true)
   bool(true)
   string(0) ""
   bool(true)
   bool(true)
   bool(false)
   bool(false)
```

Example 2 (full): {ref}`sample020.phpt`

`/ext/standard/tests/strings/str_shuffle.phpt` is a good example for using `EXPECTF` instead of
`EXPECT`. From time to time the algorithm used for shuffle changed and sometimes the machine used
to execute the code has influence on the result of shuffle. But it always returns a three character
string detectable by `%s` (that matches any string until the end of the line). Other scan-able
forms are `%a` for any amount of chars (at least one), `%i` for integers, `%d` for numbers
only, `%f` for floating point values, `%c` for single characters, `%x` for hexadecimal values,
`%w` for any number of whitespace characters and `%e` for `DIRECTORY_SEPARATOR` (`'\'` or
`'/'`).

*/ext/standard/tests/strings/str_shuffle.phpt*

```php

   --TEST--
   Testing str_shuffle.
   --FILE--
   <?php
   /* Do not change this test it is a README.TESTING example. */
   $s = '123';
   var_dump(str_shuffle($s));
   var_dump($s);
   ?>
   --EXPECTF--
   string(3) "%s"
   string(3) "123"
```

## `--EXPECTF_EXTERNAL--`

**Description:** Similar to `--EXPECTF--` section, but like the `--EXPECT_EXTERNAL--` section
just stating a filename where to load the expected output from.

**Required:** One of the `EXPECT` type sections is required.

**Test Script Support:** `run-tests.php`

## `--EXPECTREGEX--`

**Description:** An alternative of `--EXPECT--`. This form allows the tester to specify the result
in a regular expression.

**Required:** One of the `EXPECT` type sections is required.

**Format:** Plain text including regular expression patterns which represent data that can vary
between subsequent runs of a test or when run on different platforms.

Example 1 (snippet):

```text

   --EXPECTREGEX--
   M_E       : 2.718281[0-9]*
   M_LOG2E   : 1.442695[0-9]*
   M_LOG10E  : 0.434294[0-9]*
   M_LN2     : 0.693147[0-9]*
   M_LN10    : 2.302585[0-9]*
   M_PI      : 3.141592[0-9]*
   M_PI_2    : 1.570796[0-9]*
   M_PI_4    : 0.785398[0-9]*
   M_1_PI    : 0.318309[0-9]*
   M_2_PI    : 0.636619[0-9]*
   M_SQRTPI  : 1.772453[0-9]*
   M_2_SQRTPI: 1.128379[0-9]*
   M_LNPI    : 1.144729[0-9]*
   M_EULER   : 0.577215[0-9]*
   M_SQRT2   : 1.414213[0-9]*
   M_SQRT1_2 : 0.707106[0-9]*
   M_SQRT3   : 1.732050[0-9]*
```

Example 1 (full): {ref}`sample021.phpt`

Example 2 (snippet):

```text

   --EXPECTF--
   *** Testing imap_append() : basic functionality ***
   Create a new mailbox for test
   Create a temporary mailbox and add 0 msgs
   .. mailbox '%s' created
   Add a couple of msgs to new mailbox {%s}INBOX.%s
   bool(true)
   bool(true)
   Msg Count after append : 2
   List the msg headers
   array(2) {
     [0]=>
     string(%d) "%w%s       1)%s webmaster@something. Test message (%d chars)"
     [1]=>
     string(%d) "%w%s       2)%s webmaster@something. Another test (%d chars)"
   }
```

Example 2 (full): {ref}`sample025.phpt`

Example 3 (snippet):

```text

   --EXPECTREGEX--
   string\(4\) \"-012\"
   string\(8\) \"2d303132\"
   (string\(13\) \"   4294967284\"|string\(20\) \"18446744073709551604\")
   (string\(26\) \"20202034323934393637323834\"|string\(40\) \"3138343436373434303733373039353531363034\")
```

Example 3 (full): {ref}`sample023.phpt`

`/ext/standard/tests/strings/strings001.phpt` is a good example for using `EXPECTREGEX` instead
of `EXPECT`. This test also shows that in `EXPECTREGEX` some characters need to be escaped since
otherwise they would be interpreted as a regular expression.

*/ext/standard/tests/strings/strings001.phpt*

```php

   --TEST--
   Test whether strstr() and strrchr() are binary safe.
   --FILE--
   <?php
   /* Do not change this test it is a README.TESTING example. */
   $s = "alabala nica".chr(0)."turska panica";
   var_dump(strstr($s, "nic"));
   var_dump(strrchr($s," nic"));
   ?>
   --EXPECTREGEX--
   string\(18\) \"nica\x00turska panica\"
   string\(19\) \" nica\x00turska panica\"
```

## `--EXPECTREGEX_EXTERNAL--`

**Description:** Similar to `--EXPECTREGEX--` section, but like the `--EXPECT_EXTERNAL--`
section just stating a filename where to load the expected output from.

**Required:** One of the `EXPECT` type sections is required.

**Test Script Support:** `run-tests.php`

## `--CLEAN--`

**Description:** Code that is executed after a test completes. It's main purpose is to allow you to
clean up after yourself. You might need to remove files created during the test or close sockets or
database connections following a test. Infact, even if a test fails or encounters a fatal error
during the test, the code found in the `--CLEAN--` section will still run.

Code in the clean section is run in a completely different process than the one the test was run in.
So do not try accessing variables you created in the `--FILE--` section from inside the
`--CLEAN--` section, they won't exist.

Using the switch `--no-clean` on `run-tests.php`, you can prevent the code found in the
`--CLEAN--` section of a test from running. This allows you to inspect generated data or files
without them being removed by the `--CLEAN--` section.

**Required:** No.

**Test Script Support:** `run-tests.php`

**Format:** PHP source code enclosed by PHP tags.

Example 1 (snippet):

```php

   --CLEAN--
   <?php
   unlink(__DIR__.'/DomDocument_save_basic.tmp');
   ?>
```

Example 1 (full): {ref}`sample024.phpt`

Example 2 (snippet):

```php

   --CLEAN--
   <?php
   require_once('clean.inc');
   ?>
```

Example 2 (full): {ref}`sample025.phpt`

Example 3 (snippet):

```php

   --CLEAN--
   <?php
   $key = ftok(__DIR__.'/003.phpt', 'q');
   $s = shm_attach($key);
   shm_remove($s);
   ?>
```

Example 3 (full): {ref}`sample022.phpt`
