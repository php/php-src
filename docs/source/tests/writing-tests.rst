########################
 Writing tests
########################

php-src's test runner uses a custom format for tests, named PHPT.
Each PHPT test is composed of multiple sections,
however each test must have at least 3 sections:

- A title
- A script to test
- The expected output of the test

A complete description of the available section is available TODO.

.. toctree::

   phpt-sections/test
   phpt-sections/file

*******************
 Naming convention
*******************

The name of a test file should ideally be descriptive,
however a lot of old tests files do not follow this convention as the old naming convention was:

:Tests for a function's basic behaviour: ``<function_name>_basic.phpt`` (e.g. ``dba_open_basic.phpt``)

:Tests for a function's error behaviour: ``<function_name>_error.phpt`` (e.g. ``dba_open_error.phpt``)

:Tests for variations in a function's behaviour: ``<function_name>_variation.phpt`` (e.g. ``dba_open_variation.phpt``)

:General tests for extensions: ``<extname><no>.phpt`` (e.g. ``dba_003.phpt``)

For bugs the naming convention is as follows:

- If the bug is a GitHub issue: ``gh<issue_no>.phpt`` (e.g. ``gh9032.phpt``)
- If the bug is a bugsnet bug: ``bug<bug_no>.phpt`` (e.g. ``bug20528.phpt``)

**********************
 Writing a basic test
**********************


.. code:: php

   --TEST--
   echo - basic test for echo language construct
   --FILE--
   <?php
   echo 'This works ', 'and takes args!';
   ?>
   --EXPECT--
   This works and takes args!

******************************************
 An example of a more complicated test
******************************************

Let's write a test which checks that 64 bit integers are usable with the X function of the DateTime extension

TODO

.. code:: php

   --TEST--
   Test that X works with 64 bit integers
   --EXTENSIONS--
   date
   --SKIPIF--
   <?php
   if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
   ?>
   --FILE--
   <?php
   echo 'TODO';
   ?>
   --EXPECT--
   TODO
