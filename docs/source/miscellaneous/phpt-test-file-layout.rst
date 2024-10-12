#######################
 PHPT Test File Layout
#######################

.. contents:: Section Summary List
   :local:

**************
 ``--TEST--``
**************

Description
   Title of test as a single line short description.

Required
   Yes

Format
   Plain text. We recommend a single line only.

Example 1
   .. code:: text

      --TEST--
      Test filter_input() with GET and POST data.

*********************
 ``--DESCRIPTION--``
*********************

Description
   If your test requires more than a single line title to adequately describe it, you can use this
   section for further explanation. Multiple lines are allowed and besides being used for
   information, this section is completely ignored by the test binary.

Required
   No.

Format
   Plain text, multiple lines.

Example 1
   .. code:: text

      --DESCRIPTION--
      This test covers both valid and invalid usages of
      filter_input() with INPUT_GET and INPUT_POST data
      and several differnet filter sanitizers.

*****************
 ``--CREDITS--``
*****************

Description
   Used to credit contributors without CVS commit rights, who put their name and email on the first
   line. If the test was part of a TestFest event, then # followed by the name of the event and the
   date (YYYY-MM-DD) on the second line.

Required
   No.

   .. note::

      For newly created tests this section should no longer be included, as test authorship is
      already accurately tracked by Git. If multiple authors should be credited, the
      ``Co-authored-by`` tag in the commit message may be used.

Format
   .. code:: text

      Name Email
      [Event]

Example 1
   .. code:: text

      --CREDITS--
      Felipe Pena

Example 2
   .. code:: text

      --CREDITS--
      Zoe Slattery zoe@php.net
      # TestFest Munich 2009-05-19

****************
 ``--SKIPIF--``
****************

Description
   A condition or set of conditions used to determine if a test should be skipped. Tests that are
   only applicable to a certain platform, extension or PHP version are good reasons for using a
   ``--SKIPIF--`` section.

   A common practice for extension tests is to write your ``--SKIPIF--`` extension criteria into a
   file call skipif.inc and then including that file in the ``--SKIPIF--`` section of all your
   extension tests. This promotes the DRY principle and reduces future code maintenance.

Required
   No.

Format
   PHP code enclosed by PHP tags. If the output of this scripts starts with "skip", the test is
   skipped. If the output starts with "xfail", the test is marked as expected failure. The "xfail"
   convention is supported as of PHP 7.2.0.

Example 1
   .. code:: text

      --SKIPIF--
      <?php if (!extension_loaded("filter")) die("Skipped: filter extension required."); ?>

Example 2
   .. code:: text

      --SKIPIF--
      <?php include('skipif.inc'); ?>

Example 3
   .. code:: text

      --SKIPIF--
      <?php if (getenv('SKIP_ASAN')) die('xfail Startup failure leak'); ?>

*******************
 ``--CONFLICTS--``
*******************

Description
   This section is only relevant for parallel test execution (available as of PHP 7.4.0), and allows
   to specify conflict keys. While a test that conflicts with key K is running, no other test that
   conflicts with K is run. For tests conflicting with "all", no other tests are run in parallel.

   An alternative to have a ``--CONFLICTS--`` section is to add a file named CONFLICTS to the
   directory containing the tests. The contents of the CONFLICTS file must have the same format as
   the contents of the ``--CONFLICTS--`` section.

Required
   No.

Format
   One conflict key per line. Comment lines starting with # are also allowed.

Example 1
   .. code:: text

      --CONFLICTS--
      server
