########################
 Running the test suite
########################

php-src has a custom test runner that is the ``run-tests.php`` file at the root of the repository.

There are two ways of executing the test runner, either via the makefile, or by invoking the test runner directly.

******************
 Via the makefile
******************

After having compiled PHP a prompt to execute ``make test`` appears,
doing so will run all the tests (a bit less than 18 000 at the time of writing)
with a single thread.
This is slow, but there are various ways to speed this up!

First of all it is possible to set a ``TEST_PHP_ARGS`` variable with the make command
that will pass the defined arguments to the test runner.
One important argument is the ``-jN`` argument which allows the tests to be run in parallel (if possible).
Some other important arguments are ``-q``, which makes the test runner silent (i.e. stops prompting to send the test results via email),
and the ``--asan`` argument which is required when running tests with a build of PHP compiled with ASAN.

Secondly, one can reduce the number of tests being run by specifying which individual tests, or test folders
should be run via the ``TESTS`` variable.

******************************************
 Via direct invocation of the test runner
******************************************

TODO

*********************
 Test runner options
*********************

The test runner accepts various options that can be discovered by invoking it with ``--help``
Here is a list of the most common and useful options:

- ``-g``: to filter which tests are shown, e.g. ``-g FAIL`` to only show failing tests
- ``-x``: to skip tests marked as slow
- ``--offline``: to skip tests which require an internet connection
- ``-d``: to set an INI option, useful to enable/disable opcache and the JIT, e.g. ``-d opcache.enable=0``
