############
 --SKIPIF--
############

The ``--SKIPIF--`` section is used to skip a test when a condition, or a set of conditions, are met.
Tests that are only applicable to a certain platform are good reasons for using a ``--SKIPIF--``
section.

A common practice for extension tests is to write the ``--SKIPIF--`` criteria into a file named
``skipif.inc`` and include that file in the ``--SKIPIF--`` section of all the test of the extension.
This promotes the DRY principle and reduces future code maintenance.

********
 Format
********

PHP code enclosed by PHP tags. If the output of this scripts starts with ``"skip"``, the test is
skipped.

If the output starts with ``"xfail"``, the test is marked as expected failure.

**********
 Examples
**********

A basic SKIPIF section:

.. code:: php

   --SKIPIF--
   <?php
   if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
   ?>

Including a file:

.. code:: php

   --SKIPIF--
   <?php include('skipif.inc'); ?>

Using an XFAIL skip:

.. code:: php

   --SKIPIF--
   <?php if (getenv('SKIP_ASAN')) die('xfail Startup failure leak'); ?>
